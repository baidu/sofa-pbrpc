# -*- coding:UTF-8 -*-

# Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Python client for sofa-pbrpc.

  Features:
    * Only support blocking (sync) mode.
    * Support timeout by controller.SetTimeout().

  Example:
    from sofa.pbrpc import client
    channel = client.Channel("remotehost.example.com:1234")
    service = MyService_Stub(channel)
    controller = client.Controller()
    controller.SetTimeout(1)
    response = service.MyMethod(controller, request)
    if controller.Failed():
      print "[%d] %s" % (controller.ErrorCode(), controller.ErrorText())
'''

__author__ = 'qinzuoyan@gmail.com'

import time
import socket
import struct

from google.protobuf import service
from sofa.pbrpc import rpc_meta_pb2

class Error(service.RpcException): pass

class TimeoutError(Error): pass

class Controller(service.RpcController):

  """RPC controller implementation of `google.protobuf.service.RpcController'.
  """

  def __init__(self):
    self.is_failed = False
    self.error_code = 0
    self.error_text = ''
    self.timeout = None

  def Reset(self):
    """Resets the RpcController to its initial state.

    After the RpcController has been reset, it may be reused in
    a new call. Must not be called while an RPC is in progress.
    """
    self.is_failed = False
    self.error_code = 0
    self.error_text = ''
    self.timeout = None

  def SetTimeout(self, timeout):
    """Set timeout of a positive float expressing seconds.
    """
    if timeout <= 0:
      raise Error('Invalid timeout value, should be a positive float.')
    self.timeout = timeout

  def Failed(self):
    """Returns true if the call failed.

    After a call has finished, returns true if the call failed.
    It is usually caused by controller.SetFailed() in the server side.
    """
    return self.is_failed

  def ErrorCode(self):
    """If Failed is true, returns an interger error code.
    """
    return self.error_code

  def ErrorText(self):
    """If Failed is true, returns a human-readable description of the error.
    """
    return self.error_text

  def StartCancel(self):
    """Server side interface, not implemented.
    """
    raise NotImplementedError

  def SetFailed(self, reason):
    """Server side interface, not implemented.
    """
    raise NotImplementedError

  def IsCanceled(self):
    """Server side interface, not implemented.
    """
    raise NotImplementedError

class Connection(object):

  """Socket wrapper. This is a internal class, not for user.
  """

  BUFSIZE = 4096

  def __init__(self, address):
    pos = address.find(':')
    if pos == -1:
      raise Error('Invalid address: %s' % address)
    self.host = address[0:pos]
    port_str = address[pos+1:]
    try:
      self.port = int(port_str)
    except ValueError:
      raise Error('Invalid address: port should be number: %s' % port_str)
    if self.port < 0 or self.port > 65535:
      raise Error('Invalid address: port out of range: %s' % self.port)
    self.conn = None
    self.timeout = None

  def Close(self):
    """Close the socket.
    """
    if self.conn != None:
      self.conn.close()
      self.conn = None
      self.timeout = None

  def UpdateTimeout(self, deadline):
    """Update the socket timeout according to the deadline.
    """
    if deadline != None:
      now = time.time()
      if deadline <= now:
        self.Close()
        raise TimeoutError('Already timeout')
      self.timeout = deadline - now
      self.conn.settimeout(self.timeout)
    else:
      if self.timeout != None:
        self.timeout = None
        self.conn.settimeout(None)

  def WriteData(self, data, deadline=None):
    """Write data into the socket.
    """
    if self.conn == None:
      try:
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      except socket.error as err:
        self.Close()
        raise Error('Create connection fail: %s' % err)
      try:
        self.conn.connect((self.host, self.port))
      except socket.error as err:
        self.Close()
        raise Error('Connect fail: %s' % err)
    self.UpdateTimeout(deadline)
    try:
      self.conn.sendall(data)
    except socket.timeout:
      self.Close()
      raise TimeoutError('Already timeout')
    except socket.error as err:
      self.Close()
      raise Error('Send data fail: %s' % err)

  def ReadData(self, deadline=None):
    """Read data from the socket.
    """
    self.UpdateTimeout(deadline)
    try:
      data = self.conn.recv(Connection.BUFSIZE)
    except socket.timeout:
      self.Close()
      raise TimeoutError('Already timeout')
    except socket.error as err:
      self.Close()
      raise Error('Receive data fail: %s' % err)
    if len(data) == 0:
      self.Close()
      raise Error('Connection closed by peer')
    return data

class Protocol(object):

  """Protocol wrapper. This is a internal class, not for user.
  """

  MAGIC_STR = 'SOFA'
  HEADER_SIZE = 24

  def __init__(self, address):
    self.connection = Connection(address)

  def SendRequest(self, sequence_id, deadline, method_name, request):
    """Send request to the remote peer.
    """
    meta = rpc_meta_pb2.RpcMeta()
    meta.type = rpc_meta_pb2.RpcMeta.REQUEST
    meta.sequence_id = sequence_id
    meta.method = method_name
    meta_bytes = meta.SerializeToString()
    data_bytes = request.SerializeToString()
    header_bytes = struct.pack(
        '<4siqq', Protocol.MAGIC_STR,
        len(meta_bytes), len(data_bytes), len(meta_bytes)+len(data_bytes))
    bytes = header_bytes + meta_bytes + data_bytes
    self.connection.WriteData(bytes, deadline)

  def ReceiveResponse(self, sequence_id, deadline, response_class):
    """Receive response from the remote peer.
    """
    bytes = self.connection.ReadData(deadline)
    while len(bytes) < Protocol.HEADER_SIZE:
      bytes += self.connection.ReadData(deadline)
    magic_str,meta_size,data_size,message_size = struct.unpack(
        '<4siqq', bytes[0:Protocol.HEADER_SIZE])
    if magic_str != Protocol.MAGIC_STR:
      raise Error('Invalid response header: bad magic str')
    if message_size != (meta_size + data_size):
      raise Error('Invalid response header: incorrect message size')
    while len(bytes) < (Protocol.HEADER_SIZE + message_size):
      bytes += self.connection.ReadData(deadline)
    meta = rpc_meta_pb2.RpcMeta()
    meta_start = Protocol.HEADER_SIZE
    meta.ParseFromString(bytes[meta_start:meta_start+meta_size])
    if meta.type != rpc_meta_pb2.RpcMeta.RESPONSE:
      raise Error('Invalid response meta: bad type')
    if meta.sequence_id != sequence_id:
      raise Error('Invalid response meta: unexpected sequence id')
    if meta.failed:
      return (False, meta.error_code, meta.reason)
    response = response_class()
    data_start = meta_start + meta_size
    response.ParseFromString(bytes[data_start:data_start+data_size])
    return (True, response)

class Channel(service.RpcChannel):

  """RPC channel implementation of `google.protobuf.service.RpcChannel'.
  """

  def __init__(self, address):
    """Init the channel with specified remote address.

    The address should be in format of "host:port", for example "127.0.0.1:8080".
    """
    self.protocol = Protocol(address)
    self.sequence_id = 0

  def CallMethod(self, method_descriptor, rpc_controller,
                 request, response_class, done=None):
    """Calls the method identified by the descriptor.

    Only blocking mode is supported now, so the `done' param is not used.

    If some rpc error occured, then raise exception. Especially, if timeout the
    `sofa.pbrpc.TimeoutError' will be raised.

    If no rpc error occurred, but controller.SetFailed() is invoked in the
    server side, then returns None.
    """
    if rpc_controller.timeout == None:
      deadline = None
    else:
      deadline = time.time() + rpc_controller.timeout
    self.sequence_id += 1
    self.protocol.SendRequest(
        self.sequence_id, deadline, method_descriptor.full_name, request)
    resp = self.protocol.ReceiveResponse(
        self.sequence_id, deadline, response_class)
    if resp[0]:
      return resp[1]
    else:
      rpc_controller.is_failed = True
      rpc_controller.error_code = resp[1]
      rpc_controller.error_text = resp[2]
      return None


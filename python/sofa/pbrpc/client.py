# -*- coding:UTF-8 -*-

# Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''Python client for sofa-pbrpc.

  Example:
    from sofa.pbrpc import client
    channel = client.Channel("remotehost.example.com:1234")
    controller = client.Controller()
    service = MyService_Stub(channel)
    response = service.MyMethod(controller, request)
    if controller.Failed():
      print "[%d] %s" % (controller.ErrorCode(), controller.ErrorText())
'''

__author__ = 'qinzuoyan@gmail.com'

from google.protobuf import service
from sofa.pbrpc import rpc_meta_pb2
import struct
import socket

class Error(Exception): pass

class Controller(service.RpcController):

  """RPC controller implementation of `google.protobuf.service.RpcController'.
  """

  def __init__(self):
    self.is_failed = False
    self.error_code = 0
    self.error_text = ''

  def Reset(self):
    """Resets the RpcController to its initial state.

    After the RpcController has been reset, it may be reused in
    a new call. Must not be called while an RPC is in progress.
    """
    self.is_failed = False
    self.error_code = 0
    self.error_text = ''

  def Failed(self):
    """Returns true if the call failed.

    After a call has finished, returns true if the call failed.
    It is usually caused by controller.SetFailed() in the server side.
    """
    return self.is_failed

  def ErrorCode(self):
    """If Failed is true, returns an interger error code."""
    return self.error_code

  def ErrorText(self):
    """If Failed is true, returns a human-readable description of the error."""
    return self.error_text

  def ClientSetSucceed(self):
    """Internal used method."""
    self.is_failed = False
    self.error_code = 0
    self.error_text = ''

  def ClientSetFailed(self, error_code, error_text):
    """Internal used method."""
    self.is_failed = True
    self.error_code = error_code
    self.error_text = error_text

  def StartCancel(self):
    """Only need in server side, not implemented."""
    raise NotImplementedError

  def SetFailed(self, reason):
    """Only need in server side, not implemented."""
    raise NotImplementedError

  def IsCanceled(self):
    """Only need in server side, not implemented."""
    raise NotImplementedError

class Connection(object):

  """Socket wrapper.
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

  def WriteData(self, data):
    if self.conn == None:
      try:
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      except socket.error as err:
        self.conn = None
        raise Error('Create connection fail: %s' % err)
      try:
        self.conn.connect((self.host, self.port))
      except socket.error as err:
        self.conn.close()
        self.conn = None
        raise Error('Connect fail: %s' % err)
    try:
      self.conn.sendall(data)
    except socket.error as err:
      self.conn.close()
      self.conn = None
      raise Error('Send data fail: %s' % err)

  def ReadData(self):
    try:
      data = self.conn.recv(Connection.BUFSIZE)
    except socket.error as err:
      self.conn = None
      raise Error('Receive data fail: %s' % err)
    if len(data) == 0:
      self.conn.close()
      self.conn = None
      raise Error('Connection closed by peer')
    return data

class Protocol(object):

  """Protocol wrapper.
  """

  MAGIC_STR = 'SOFA'
  HEADER_SIZE = 24

  def __init__(self, address):
    self.connection = Connection(address)

  def SendRequest(self, sequence_id, method_name, request):
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
    self.connection.WriteData(bytes)

  def ReceiveResponse(self, sequence_id, response_class):
    bytes = self.connection.ReadData()
    while len(bytes) < Protocol.HEADER_SIZE:
      bytes += self.connection.ReadData()
    magic_str,meta_size,data_size,message_size = struct.unpack(
        '<4siqq', bytes[0:Protocol.HEADER_SIZE])
    if magic_str != Protocol.MAGIC_STR:
      raise Error('Invalid response header: bad magic str')
    if message_size != (meta_size + data_size):
      raise Error('Invalid response header: incorrect message size')
    while len(bytes) < (Protocol.HEADER_SIZE + message_size):
      bytes += self.connection.ReadData()
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

    Call the given method of the remote service.  The signature of this
    procedure looks the same as Service.CallMethod(), but the requirements
    are less strict in one important way:  the request object doesn't have to
    be of any specific class as long as its descriptor is method.input_type.
    """
    self.sequence_id += 1
    self.protocol.SendRequest(self.sequence_id,
        method_descriptor.full_name, request)
    resp = self.protocol.ReceiveResponse(self.sequence_id, response_class)
    if resp[0]:
      rpc_controller.ClientSetSucceed()
      return resp[1]
    else:
      rpc_controller.ClientSetFailed(resp[1], resp[2])
      return None


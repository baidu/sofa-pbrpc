# -*- coding:UTF-8 -*-

# Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# This is a sample code to show how to use python client of sofa-pbrpc.
#
# Preconditions:
#   * The protobuf python lib has been installed.
#   * The sofa-pbrpc python lib has been installed.
#   * The server in ../../sample/echo has been started.

from sofa.pbrpc import client
import echo_service_pb2

channel = client.Channel('127.0.0.1:12321')
service = echo_service_pb2.EchoServer_Stub(channel)

controller = client.Controller()
request = echo_service_pb2.EchoRequest()
request.message = 'Hello World'
response = service.Echo(controller, request)

if controller.Failed():
  print "ERROR: [%d] %s" % (controller.ErrorCode(), controller.ErrorText())
else:
  print "Response: %s" % response


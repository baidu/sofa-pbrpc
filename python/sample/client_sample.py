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
#
# For more, please refer to `./README'.

from sofa.pbrpc import client
import echo_service_pb2
import sys

# Create service stub.
channel = client.Channel('127.0.0.1:12321')
service = echo_service_pb2.EchoServer_Stub(channel)

# Create controller.
# We set timeout to 1.5 seconds by controller.SetTimeout() method.
controller = client.Controller()
controller.SetTimeout(1.5)

# Prepare request.
request = echo_service_pb2.EchoRequest()
request.message = 'Hello World'

# Call method.
try:
  response = service.Echo(controller, request)
except client.TimeoutError:
  print "ERROR: RPC timeout"
  sys.exit(1)
except Exception as e:
  print "ERROR: RPC fail: %s" % e
  sys.exit(1)

# Check remote failure.
if controller.Failed():
  print "ERROR: Remote fail: %s" % controller.ErrorText()
  sys.exit(1)

# OK, print response.
print "Response:\n\n%s" % response


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
import urllib2

# Prepare post data
echo_request = echo_service_pb2.EchoRequest()
echo_request.message = 'Hello World'
send_data = echo_request.SerializeToString()

# Prepare http request
url = 'http://localhost:12321/sofa.pbrpc.test.EchoServer.Echo'
accept = 'application/protobuf'
headers = { 'Accept' : accept }
request = urllib2.Request(url, send_data, headers)

# Send request 
try:
    response = urllib2.urlopen(request)
except Exception as e:
    print "ERROR: Send fail: %s" % e.reason
    sys.exit(1)

# Read http body
recv_data = response.read()

# check failure
if response.info().getheader('Content-Type') != accept:
    print "ERROR: %s" % recv_data
    sys.exit(1)

# print response
echo_response = echo_service_pb2.EchoResponse()
echo_response.ParseFromString(recv_data)
print "Response:\n%s" % echo_response.message

<?php
// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

require_once 'echo_service.pb.php';

$request = new EchoRequest();
$request->set_message('Hello from qinzuoyan01');
$response = new EchoResponse();

$stub = new EchoServer("127.0.0.1:12321");
$stub->SetTimeout(3000);
$closure = NULL;

$stub->TestEcho($request, $response, $closure);
if (!$stub->Failed()) 
{
    echo "request succeed : " . $response->get_message() . "\n";
}
else
{
    echo $stub->ErrorText() . "\n";
}

?>

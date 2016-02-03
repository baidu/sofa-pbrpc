<?php
// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

require_once 'test.pb.php';

ini_set('max_execution_time', '10000000');

$request = new Request();
$pairs = array();
$pair_a = new TestPair();
$pair_a->set_id("1");
$pair_a->set_value("pair_a");
$pairs[] = $pair_a;
$pair_b = new TestPair();
$pair_b->set_id("2");
$pair_b->set_value("pair_b");
$pairs[] = $pair_b;
$request->set_pair($pairs);
$request->set_debug_message("Debug message from client");
$response = new Response();

$stub = new TestService("127.0.0.1:9008");
$stub->SetTimeout(3000);
$closure = NULL;

for($i=0; $i < 10000; ++$i)
{
    $stub->test_func($request, $response, $closure);
    if (!$stub->Failed()) 
    {
        $debug_msg = $response->get_debug_message();
        $result = $response->get_resultObjectAt(0); 
        $result_index = $result->get_index();
        $result_data = $result->get_data();

        echo "reveived debug message:" . $debug_msg . "\n";
        echo "received result index:" . $result_index . "\n";
        echo "received result data:" . $result_data . "\n";
    }
    else
    {
        echo $stub->ErrorText() . "\n";
    }
}

?>

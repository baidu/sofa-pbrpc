<?php
/* * *****************************************************************************
* 
* Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
* 
 * **************************************************************************** */
 
/**
* @File: test.php
* @Author: zhangdi05(zhangdi05@baidu.com)
* @Date: 2015/11/16 21:26:19
* @Brief: 
*/
$curl = curl_init();
// 设置你需要抓取的URL 
curl_setopt($curl, CURLOPT_URL, 'http://www.cmx8.cn');
// 设置header 
curl_setopt($curl, CURLOPT_HEADER, 1);
// 设置cURL 参数，要求结果保存到字符串中还是输出到屏幕上。 
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
// 运行cURL，请求网页 
$data = curl_exec($curl);
// 关闭URL请求 
curl_close($curl);
// 显示获得的数据 
var_dump($data);
?>
 

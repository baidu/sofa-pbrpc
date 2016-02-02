--TEST--
XML_Util::apiVersion() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::apiVersion() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  basic apiVersion() call" . PHP_EOL;
echo XML_Util::apiVersion() . PHP_EOL;
?>
--EXPECT--
=====XML_Util::apiVersion() basic tests=====

TEST:  basic apiVersion() call
1.1

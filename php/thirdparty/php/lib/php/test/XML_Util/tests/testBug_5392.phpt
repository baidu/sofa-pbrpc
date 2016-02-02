--TEST--
XML_Util tests for Bug #5392 "encoding of ISO-8859-1 is the only supported encoding"
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util tests for Bug #5392 "encoding of ISO-8859-1 is the only supported encoding"=====' . PHP_EOL . PHP_EOL;

echo "TEST:  test case provided in bug report" . PHP_EOL;
$data = 'This data contains special chars like <, >, & and " as well as ä, ö, ß, à and ê';

$replaced = XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_HTML, 'UTF-8');

$reversed = XML_Util::reverseEntities($replaced, XML_UTIL_ENTITIES_HTML, 'UTF-8');

echo $replaced . PHP_EOL;
echo $reversed . PHP_EOL;

?>
--EXPECT--
=====XML_Util tests for Bug #5392 "encoding of ISO-8859-1 is the only supported encoding"=====

TEST:  test case provided in bug report
This data contains special chars like &lt;, &gt;, &amp; and &quot; as well as &auml;, &ouml;, &szlig;, &agrave; and &ecirc;
This data contains special chars like <, >, & and " as well as ä, ö, ß, à and ê

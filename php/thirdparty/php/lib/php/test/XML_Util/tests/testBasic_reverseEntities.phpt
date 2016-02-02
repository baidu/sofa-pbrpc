--TEST--
XML_Util::reverseEntities() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::reverseEntities() basic tests=====' . PHP_EOL . PHP_EOL;

$data = 'This string contains &lt; &amp; &gt;.';
$utf8 = 'This data contains special chars like &lt;, &gt;, &amp; and &quot; as well as &auml;, &ouml;, &szlig;, &agrave; and &ecirc;';

echo "TEST:  basic usage" . PHP_EOL;
echo XML_Util::reverseEntities($data) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage but with bogus \$replaceEntities arg" . PHP_EOL;
echo XML_Util::reverseEntities($data, 'I_AM_BOGUS') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_XML) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_XML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_XML and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($utf8, XML_UTIL_ENTITIES_XML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML_REQUIRED" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_XML_REQUIRED) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML_REQUIRED and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_XML_REQUIRED, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_XML_REQUIRED and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($utf8, XML_UTIL_ENTITIES_XML_REQUIRED, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_HTML" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_HTML) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_HTML and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($data, XML_UTIL_ENTITIES_HTML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_HTML and UTF-8" . PHP_EOL;
echo XML_Util::reverseEntities($utf8, XML_UTIL_ENTITIES_HTML, 'UTF-8') . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::reverseEntities() basic tests=====

TEST:  basic usage
This string contains < & >.

TEST:  basic usage but with bogus $replaceEntities arg
This string contains &lt; &amp; &gt;.

TEST:  basic usage with ENTITIES_XML
This string contains < & >.

TEST:  basic usage with ENTITIES_XML and UTF-8
This string contains < & >.

TEST:  utf8 usage with ENTITIES_XML and UTF-8
This data contains special chars like <, >, & and " as well as &auml;, &ouml;, &szlig;, &agrave; and &ecirc;

TEST:  basic usage with ENTITIES_XML_REQUIRED
This string contains < & &gt;.

TEST:  basic usage with ENTITIES_XML_REQUIRED and UTF-8
This string contains < & &gt;.

TEST:  utf8 usage with ENTITIES_XML_REQUIRED and UTF-8
This data contains special chars like <, &gt;, & and " as well as &auml;, &ouml;, &szlig;, &agrave; and &ecirc;

TEST:  basic usage with ENTITIES_HTML
This string contains < & >.

TEST:  basic usage with ENTITIES_HTML and UTF-8
This string contains < & >.

TEST:  utf8 usage with ENTITIES_HTML and UTF-8
This data contains special chars like <, >, & and " as well as ä, ö, ß, à and ê

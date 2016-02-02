--TEST--
XML_Util::replaceEntities() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::replaceEntities() basic tests=====' . PHP_EOL . PHP_EOL;

$data = 'This string contains < & >.';
$utf8 = 'This data contains special chars like <, >, & and " as well as ä, ö, ß, à and ê';

echo "TEST:  basic usage" . PHP_EOL;
echo XML_Util::replaceEntities($data) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage but with bogus \$replaceEntities arg" . PHP_EOL;
echo XML_Util::replaceEntities($data, 'I_AM_BOGUS') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_XML) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_XML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_XML and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($utf8, XML_UTIL_ENTITIES_XML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML_REQUIRED" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_XML_REQUIRED) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_XML_REQUIRED and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_XML_REQUIRED, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_XML_REQUIRED and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($utf8, XML_UTIL_ENTITIES_XML_REQUIRED, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_HTML" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_HTML) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with ENTITIES_HTML and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($data, XML_UTIL_ENTITIES_HTML, 'UTF-8') . PHP_EOL . PHP_EOL;

echo "TEST:  utf8 usage with ENTITIES_HTML and UTF-8" . PHP_EOL;
echo XML_Util::replaceEntities($utf8, XML_UTIL_ENTITIES_HTML, 'UTF-8') . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::replaceEntities() basic tests=====

TEST:  basic usage
This string contains &lt; &amp; &gt;.

TEST:  basic usage but with bogus $replaceEntities arg
This string contains < & >.

TEST:  basic usage with ENTITIES_XML
This string contains &lt; &amp; &gt;.

TEST:  basic usage with ENTITIES_XML and UTF-8
This string contains &lt; &amp; &gt;.

TEST:  utf8 usage with ENTITIES_XML and UTF-8
This data contains special chars like &lt;, &gt;, &amp; and &quot; as well as ä, ö, ß, à and ê

TEST:  basic usage with ENTITIES_XML_REQUIRED
This string contains &lt; &amp; >.

TEST:  basic usage with ENTITIES_XML_REQUIRED and UTF-8
This string contains &lt; &amp; >.

TEST:  utf8 usage with ENTITIES_XML_REQUIRED and UTF-8
This data contains special chars like &lt;, >, &amp; and &quot; as well as ä, ö, ß, à and ê

TEST:  basic usage with ENTITIES_HTML
This string contains &lt; &amp; &gt;.

TEST:  basic usage with ENTITIES_HTML and UTF-8
This string contains &lt; &amp; &gt;.

TEST:  utf8 usage with ENTITIES_HTML and UTF-8
This data contains special chars like &lt;, &gt;, &amp; and &quot; as well as &auml;, &ouml;, &szlig;, &agrave; and &ecirc;


--TEST--
XML_Util::collapseEmptyTags() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::collapseEmptyTags() basic tests=====' . PHP_EOL . PHP_EOL;

$emptyTag = "<foo></foo>";
$otherTag = "<bar>baz</bar>";
$xhtmlTag = "<b></b>";

echo "TEST:  basic usage" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage alongside non-empty tag" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag . $otherTag) . PHP_EOL . PHP_EOL;

echo "TEST:  one empty tag, with COLLAPSE_ALL set" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag, XML_UTIL_COLLAPSE_ALL) . PHP_EOL . PHP_EOL;

echo "TEST:  one empty tag alongside non-empty tag, with COLLAPSE_ALL set" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag . $otherTag, XML_UTIL_COLLAPSE_ALL) . PHP_EOL . PHP_EOL;

echo "TEST:  one empty tag, with COLLAPSE_XHTML_ONLY set" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag, XML_UTIL_COLLAPSE_XHTML_ONLY) . PHP_EOL . PHP_EOL;

echo "TEST:  one empty tag alongside non-empty tag, with COLLAPSE_XHTML_ONLY set" . PHP_EOL;
echo XML_Util::collapseEmptyTags($emptyTag . $xhtmlTag . $otherTag, XML_UTIL_COLLAPSE_XHTML_ONLY) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::collapseEmptyTags() basic tests=====

TEST:  basic usage
<foo />

TEST:  basic usage alongside non-empty tag
<foo /><bar>baz</bar>

TEST:  one empty tag, with COLLAPSE_ALL set
<foo />

TEST:  one empty tag alongside non-empty tag, with COLLAPSE_ALL set
<foo /><bar>baz</bar>

TEST:  one empty tag, with COLLAPSE_XHTML_ONLY set
<foo></foo>

TEST:  one empty tag alongside non-empty tag, with COLLAPSE_XHTML_ONLY set
<foo></foo><b></b><bar>baz</bar>

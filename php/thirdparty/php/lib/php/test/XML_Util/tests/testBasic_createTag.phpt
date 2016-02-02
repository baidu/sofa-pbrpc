--TEST--
XML_Util::createTag() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::createTag() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar")
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute and content" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag"
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, and namespace" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag",
    "http://www.w3c.org/myNs#"
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, and REPLACE_ENTITIES" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, and CDATA_SECTION" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_CDATA_SECTION
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, and multiline = false" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    false
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, and multiline = true" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    true
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, and indent = (2 spaces)" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    true,
    '  '
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), and linebreak = '^'" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    true,
    '  ',
    '^'
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = true" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar", "boo" => "baz"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    true,
    '  ',
    '^',
    true
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = false" . PHP_EOL;
echo XML_Util::createTag(
    "myNs:myTag", 
    array("foo" => "bar", "boo" => "baz"), 
    "This is inside the tag and has < & @ > in it",
    "http://www.w3c.org/myNs#",
    XML_UTIL_REPLACE_ENTITIES,
    true,
    '  ',
    '^',
    false
) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::createTag() basic tests=====

TEST:  tag with attribute
<myNs:myTag foo="bar" />

TEST:  tag with attribute and content
<myNs:myTag foo="bar">This is inside the tag</myNs:myTag>

TEST:  tag with attribute, content, and namespace
<myNs:myTag foo="bar" xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag</myNs:myTag>

TEST:  tag with attribute, content, namespace, and REPLACE_ENTITIES
<myNs:myTag foo="bar" xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, and CDATA_SECTION
<myNs:myTag foo="bar" xmlns:myNs="http://www.w3c.org/myNs#"><![CDATA[This is inside the tag and has < & @ > in it]]></myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, and multiline = false
<myNs:myTag foo="bar" xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, and multiline = true
<myNs:myTag foo="bar"
            xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, and indent = (2 spaces)
<myNs:myTag foo="bar"
  xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), and linebreak = '^'
<myNs:myTag foo="bar"^  xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = true
<myNs:myTag boo="baz"^  foo="bar"^  xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

TEST:  tag with attribute, content, namespace, REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = false
<myNs:myTag foo="bar"^  boo="baz"^  xmlns:myNs="http://www.w3c.org/myNs#">This is inside the tag and has &lt; &amp; @ &gt; in it</myNs:myTag>

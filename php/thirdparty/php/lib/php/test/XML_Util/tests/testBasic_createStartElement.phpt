--TEST--
XML_Util::createStartElement() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::createStartElement() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  tag only" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag"
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar")
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag only, passing '' as attribute arg" . PHP_EOL;
echo XML_Util::createStartElement(
    'myNs:myTag',
    ''
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes and namespace" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar"),
    "http://www.w3c.org/myNs#"
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with empty attributes, whose namespaceUri is not a full namespace" . PHP_EOL;
echo XML_Util::createStartElement(
    'myTag',
    '',
    'foo'
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes, namespace, and multiline = true" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar"),
    "http://www.w3c.org/myNs#",
    true
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes, namespace, multiline = true, and indent = (2 spaces only)" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar"),
    "http://www.w3c.org/myNs#",
    true,
    '  '
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), and linebreak = '^'" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar"),
    "http://www.w3c.org/myNs#",
    true,
    '  ',
    '^'
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), linebreak = '^', and sortAttributes = true" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar", "boo" => "baz"),
    "http://www.w3c.org/myNs#",
    true,
    '  ',
    '^',
    true
) . PHP_EOL . PHP_EOL;

echo "TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), linebreak = '^', and sortAttributes = false" . PHP_EOL;
echo XML_Util::createStartElement(
    "myNs:myTag", 
    array("foo" => "bar", "boo" => "baz"),
    "http://www.w3c.org/myNs#",
    true,
    '  ',
    '^',
    false
) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::createStartElement() basic tests=====

TEST:  tag only
<myNs:myTag>

TEST:  tag with attributes
<myNs:myTag foo="bar">

TEST:  tag only, passing '' as attribute arg
<myNs:myTag>

TEST:  tag with attributes and namespace
<myNs:myTag foo="bar" xmlns:myNs="http://www.w3c.org/myNs#">

TEST:  tag with empty attributes, whose namespaceUri is not a full namespace
<myTag xmlns="foo">

TEST:  tag with attributes, namespace, and multiline = true
<myNs:myTag foo="bar"
            xmlns:myNs="http://www.w3c.org/myNs#">

TEST:  tag with attributes, namespace, multiline = true, and indent = (2 spaces only)
<myNs:myTag foo="bar"
  xmlns:myNs="http://www.w3c.org/myNs#">

TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), and linebreak = '^'
<myNs:myTag foo="bar"^  xmlns:myNs="http://www.w3c.org/myNs#">

TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), linebreak = '^', and sortAttributes = true
<myNs:myTag boo="baz"^  foo="bar"^  xmlns:myNs="http://www.w3c.org/myNs#">

TEST:  tag with attributes, namespace, multiline = true, indent = (2 spaces only), linebreak = '^', and sortAttributes = false
<myNs:myTag foo="bar"^  boo="baz"^  xmlns:myNs="http://www.w3c.org/myNs#">

--TEST--
XML_Util::createTagFromArray() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::createTagFromArray() basic tests=====' . PHP_EOL . PHP_EOL;

$bad = array(
    "foo" => "bar",
);
$tag1 = array(
    "qname"        => "foo:bar",
);
$tag2 = array(
    "qname"        => "foo:bar",
    "namespaceUri" => "http://foo.com",
);
$tag3 = array(
    "qname"        => "foo:bar",
    "namespaceUri" => "http://foo.com",
    "attributes"   => array( "key" => "value", "argh" => "fruit&vegetable" ),
);
$tag4 = array(
    "qname"        => "foo:bar",
    "namespaceUri" => "http://foo.com",
    "attributes"   => array( "key" => "value", "argh" => "fruit&vegetable" ),
    "content"      => "I'm inside the tag",
);
$tag5 = array(
    "qname"        => "foo:bar",
    "attributes"   => array( "key" => "value", "argh" => "fruit&vegetable" ),
    "content"      => "I'm inside the tag",
);
$tag6 = array(
    "qname"        => "foo:bar",
    "namespaceUri" => "http://foo.com",
    "content"      => "I'm inside the tag",
);
$tag7 = array(
    "namespaceUri" => "http://foo.com",
    "attributes"   => array( "key" => "value", "argh" => "fruit&vegetable" ),
    "content"      => "I'm inside the tag",
);

$tag8 = array(
    'content'      => array('foo', 'bar')
);

$tag9 = array(
    'qname'        => 'foo:bar',
    'namespaces'   => array('ns1' => 'uri1', 'ns2' => 'uri2')
);

$tag10 = array(
    'namespace'    => 'http://foo.org',
    'localPart'    => 'bar'
);

$tag11 = array(
    'namespace'    => '',
    'localPart'    => 'bar'
);

$tag12 = array(
    'localPart'    => 'foo',
    'namespaceUri' => 'http://bar.org'
);

echo "TEST:  basic usage with an invalid array" . PHP_EOL;
echo XML_Util::createTagFromArray($bad) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname only)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag1) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname and namespaceUri)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag2) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, and attributes)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag3) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, attributes, and content)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag5) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, and content)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag6) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (namespaceUri, attributes, and content)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag7) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), plus REPLACE_ENTITIES" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), plus ENTITIES_NONE" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_ENTITIES_NONE) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, and multiline = false" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, false) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, and multiline = true" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, true) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, and indent = (2 spaces)" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, true, '  ') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), and linebreak = '^'" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, true, '  ', '^') . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = true" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, true, '  ', '^', true) . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = false" . PHP_EOL;
echo XML_Util::createTagFromArray($tag4, XML_UTIL_REPLACE_ENTITIES, true, '  ', '^', false) . PHP_EOL . PHP_EOL;

echo 'TEST:  cause a non-scalar error on the content tag' . PHP_EOL;
echo XML_Util::createTagFromArray($tag8) . PHP_EOL . PHP_EOL;

echo 'TEST:  handle an array of namespaces being passed' . PHP_EOL;
echo XML_Util::createTagFromArray($tag9) . PHP_EOL . PHP_EOL;

echo 'TEST:  qname is derived from namespace + localPart' . PHP_EOL;
echo XML_Util::createTagFromArray($tag10) . PHP_EOL . PHP_EOL;

echo 'TEST:  qname is derived from localPart only' . PHP_EOL;
echo XML_Util::createTagFromArray($tag11) . PHP_EOL . PHP_EOL;

echo 'TEST:  namespaceUri is given, but namespace is not' . PHP_EOL;
echo XML_Util::createTagFromArray($tag12) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::createTagFromArray() basic tests=====

TEST:  basic usage with an invalid array
You must either supply a qualified name (qname) or local tag name (localPart).

TEST:  basic usage with a valid array (qname only)
<foo:bar />

TEST:  basic usage with a valid array (qname and namespaceUri)
<foo:bar xmlns:foo="http://foo.com" />

TEST:  basic usage with a valid array (qname, namespaceUri, and attributes)
<foo:bar argh="fruit&amp;vegetable" key="value" xmlns:foo="http://foo.com" />

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content)
<foo:bar argh="fruit&amp;vegetable" key="value" xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, attributes, and content)
<foo:bar argh="fruit&amp;vegetable" key="value">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, and content)
<foo:bar xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (namespaceUri, attributes, and content)
You must either supply a qualified name (qname) or local tag name (localPart).

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), plus REPLACE_ENTITIES
<foo:bar argh="fruit&amp;vegetable" key="value" xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), plus ENTITIES_NONE
<foo:bar argh="fruit&amp;vegetable" key="value" xmlns:foo="http://foo.com">I'm inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, and multiline = false
<foo:bar argh="fruit&amp;vegetable" key="value" xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, and multiline = true
<foo:bar argh="fruit&amp;vegetable"
         key="value"
         xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, and indent = (2 spaces)
<foo:bar argh="fruit&amp;vegetable"
  key="value"
  xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), and linebreak = '^'
<foo:bar argh="fruit&amp;vegetable"^  key="value"^  xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = true
<foo:bar argh="fruit&amp;vegetable"^  key="value"^  xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  basic usage with a valid array (qname, namespaceUri, attributes, and content), REPLACE_ENTITIES, multiline = true, indent = (2 spaces), linebreak = '^', and sortAttributes = false
<foo:bar key="value"^  argh="fruit&amp;vegetable"^  xmlns:foo="http://foo.com">I&apos;m inside the tag</foo:bar>

TEST:  cause a non-scalar error on the content tag
Supplied non-scalar value as tag content

TEST:  handle an array of namespaces being passed
<foo:bar xmlns:ns1="uri1" xmlns:ns2="uri2" />

TEST:  qname is derived from namespace + localPart
<http://foo.org:bar />

TEST:  qname is derived from localPart only
<bar />

TEST:  namespaceUri is given, but namespace is not
<foo xmlns="http://bar.org" />

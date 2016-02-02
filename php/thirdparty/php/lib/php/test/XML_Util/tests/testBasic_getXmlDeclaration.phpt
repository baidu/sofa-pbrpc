--TEST--
XML_Util::getXmlDeclaration() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::getXmlDeclaration() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  using version only" . PHP_EOL;
echo XML_Util::getXMLDeclaration("1.0") . PHP_EOL . PHP_EOL;

echo "TEST:  using version and encoding" . PHP_EOL;
echo XML_Util::getXMLDeclaration("1.0", "UTF-8") . PHP_EOL . PHP_EOL;

echo "TEST:  using version, encoding, and standalone flag" . PHP_EOL;
echo XML_Util::getXMLDeclaration("1.0", "UTF-8", true) . PHP_EOL . PHP_EOL;

echo "TEST:  using version and standalone flag" . PHP_EOL;
echo XML_Util::getXMLDeclaration("1.0", null, true) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::getXmlDeclaration() basic tests=====

TEST:  using version only
<?xml version="1.0"?>

TEST:  using version and encoding
<?xml version="1.0" encoding="UTF-8"?>

TEST:  using version, encoding, and standalone flag
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>

TEST:  using version and standalone flag
<?xml version="1.0" standalone="yes"?>

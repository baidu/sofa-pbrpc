--TEST--
XML_Util::getDocTypeDeclaration() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::getDocTypeDeclaration() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  using root only" . PHP_EOL;
echo XML_Util::getDocTypeDeclaration("rootTag") . PHP_EOL . PHP_EOL;

echo "TEST:  using root and a string URI" . PHP_EOL;
echo XML_Util::getDocTypeDeclaration("rootTag", "myDocType.dtd") . PHP_EOL . PHP_EOL;

$uri = array(
    'uri' => 'http://pear.php.net/dtd/package-1.0',
    'id' => '-//PHP//PEAR/DTD PACKAGE 0.1'
);
$dtdEntry = '<!ELEMENT additionalInfo (#PCDATA)>';

echo "TEST:  using root and an array URI" . PHP_EOL;
echo XML_Util::getDocTypeDeclaration("rootTag", $uri) . PHP_EOL . PHP_EOL;

echo "TEST:  using root and an array URI and an internal DTD entry" . PHP_EOL;
echo XML_Util::getDocTypeDeclaration("rootTag", $uri, $dtdEntry) . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::getDocTypeDeclaration() basic tests=====

TEST:  using root only
<!DOCTYPE rootTag>

TEST:  using root and a string URI
<!DOCTYPE rootTag SYSTEM "myDocType.dtd">

TEST:  using root and an array URI
<!DOCTYPE rootTag PUBLIC "-//PHP//PEAR/DTD PACKAGE 0.1" "http://pear.php.net/dtd/package-1.0">

TEST:  using root and an array URI and an internal DTD entry
<!DOCTYPE rootTag PUBLIC "-//PHP//PEAR/DTD PACKAGE 0.1" "http://pear.php.net/dtd/package-1.0" [
<!ELEMENT additionalInfo (#PCDATA)>
]>

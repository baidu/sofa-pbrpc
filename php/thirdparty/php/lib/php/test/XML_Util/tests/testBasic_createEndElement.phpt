--TEST--
XML_Util::createEndElement() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::createEndElement() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage (myTag)" . PHP_EOL;
echo XML_Util::createEndElement("myTag") . PHP_EOL . PHP_EOL;

echo "TEST:  basic usage with a namespaced tag (myNs:myTag)" . PHP_EOL;
echo XML_Util::createEndElement("myNs:myTag") . PHP_EOL . PHP_EOL;
?>
--EXPECT--
=====XML_Util::createEndElement() basic tests=====

TEST:  basic usage (myTag)
</myTag>

TEST:  basic usage with a namespaced tag (myNs:myTag)
</myNs:myTag>

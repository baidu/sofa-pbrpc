--TEST--
XML_Util::isValidName() basic tests
--CREDITS--
Chuck Burgess <ashnazg@php.net>
# created for v1.2.0a1 2008-05-04
--FILE--
<?php
require_once 'XML' . DIRECTORY_SEPARATOR . 'Util.php';
echo '=====XML_Util::isValidName() basic tests=====' . PHP_EOL . PHP_EOL;

echo "TEST:  valid tag" . PHP_EOL;
$result = XML_Util::isValidName("alpha-x_y_z.123");
if (is_a($result, 'PEAR_Error')) {
    print "Invalid XML name: " . $result->getMessage() . PHP_EOL . PHP_EOL;
} else {
    print "Valid XML name." . PHP_EOL . PHP_EOL;
}

echo "TEST:  invalid tag" . PHP_EOL;
$result = XML_Util::isValidName("invalidTag?");
if (is_a($result, 'PEAR_Error')) {
    print "Invalid XML name: " . $result->getMessage() . PHP_EOL . PHP_EOL;
} else {
    print "Valid XML name." . PHP_EOL . PHP_EOL;
}

echo "TEST:  invalid tag that doesn't start with a letter" . PHP_EOL;
$result = XML_Util::isValidName("1234five");
if (is_a($result, 'PEAR_Error')) {
    print "Invalid XML name: " . $result->getMessage() . PHP_EOL . PHP_EOL;
} else {
    print "Valid XML name." . PHP_EOL . PHP_EOL;
}

?>
--EXPECT--
=====XML_Util::isValidName() basic tests=====

TEST:  valid tag
Valid XML name.

TEST:  invalid tag
Invalid XML name: XML names may only contain alphanumeric chars, period, hyphen, colon and underscores

TEST:  invalid tag that doesn't start with a letter
Invalid XML name: XML names may only start with letter or underscore

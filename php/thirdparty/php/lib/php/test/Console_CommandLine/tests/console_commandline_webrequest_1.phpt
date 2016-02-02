--TEST--
Test for Console_CommandLine::parse() with a web request 1
--GET--
version=1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser1();
$parser->parse();

?>
--EXPECT--
some_program version 0.1.0.

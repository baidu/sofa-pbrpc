--TEST--
Test for Console_CommandLine::addOption() method (errors 1).
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addOption('Some invalid name');

?>
--EXPECTF--

Fatal error: option name must be a valid php variable name (got: Some invalid name) in %sCommandLine.php on line %d

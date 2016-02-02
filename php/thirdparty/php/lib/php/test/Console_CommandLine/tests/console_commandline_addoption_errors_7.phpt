--TEST--
Test for Console_CommandLine::addOption() method (errors 7).
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addOption('name', array('short_name'=>'-d', 'action'=>'Callback'));

?>
--EXPECTF--

Fatal error: you must provide a valid callback for option "name" in %sCommandLine.php on line %d

--TEST--
Test for Console_CommandLine::addOption() method (errors 5).
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addOption('name', array('short_name'=>'-d', 'action'=>true));

?>
--EXPECTF--

Fatal error: invalid action for option "name". in %sCommandLine.php on line %d

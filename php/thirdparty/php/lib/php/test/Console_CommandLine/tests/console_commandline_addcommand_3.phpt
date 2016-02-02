--TEST--
Test for Console_CommandLine::addCommand() method.
--ARGS--
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine(array('subcommand_required' => true));
$parser->addCommand('cmd1');
$parser->addCommand('cmd2');
$parser->addCommand('cmd3');
try {
    $parser->parse();
} catch (Console_CommandLine_Exception $exc) {
    echo $exc->getMessage();
}

?>
--EXPECTF--
Please enter one of the following command: cmd1, cmd2, cmd3.

--TEST--
Test for Console_CommandLine::addArgument() method.
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
foo
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addArgument('arg1');
$parser->addArgument('arg2', array(
    'optional' => true,
    'default' => 'bar'
));

$result = $parser->parse();
echo $result->args['arg1'] . ' ' . $result->args['arg2'];

// a bad argument
$parser->addArgument('arg3', array('default' => 'baz'));

?>
--EXPECTF--
foo bar
Fatal error: only optional arguments can have a default value in %sCommandLine.php on line %d

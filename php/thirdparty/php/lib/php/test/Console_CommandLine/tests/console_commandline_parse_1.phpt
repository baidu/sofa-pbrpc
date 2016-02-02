--TEST--
Test for Console_CommandLine::parse() method (--version).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
--version
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser1();
$parser->parse();

?>
--EXPECT--
some_program version 0.1.0.

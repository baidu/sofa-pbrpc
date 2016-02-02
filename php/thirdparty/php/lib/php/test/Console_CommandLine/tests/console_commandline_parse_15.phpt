--TEST--
Test for Console_CommandLine::parse() method (subcommand error).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
install -f 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser2();
try {
    $result = $parser->parse();
} catch (Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>
--EXPECT--
Error: You must provide at least 1 argument.
Type "some_program --help" to get help.
Type "some_program <command> --help" to get help on specific command.

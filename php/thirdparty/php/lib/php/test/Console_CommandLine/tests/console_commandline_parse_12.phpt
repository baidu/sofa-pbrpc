--TEST--
Test for Console_CommandLine::parse() method (subcommand help 2).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
inst --help 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser2();
$result = $parser->parse();

?>
--EXPECT--
install given package

Usage:
  some_program [options] install [options] package

Options:
  -f, --force  force installation
  -h, --help   show this help message and exit

Arguments:
  package  package to install

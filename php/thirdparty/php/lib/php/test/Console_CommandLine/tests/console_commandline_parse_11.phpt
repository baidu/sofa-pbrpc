--TEST--
Test for Console_CommandLine::parse() method (subcommand help 1).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
--help 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser2();
$result = $parser->parse();

?>
--EXPECT--
Description of our parser goes here...

Usage:
  some_program [options]
  some_program [options] <command> [options] [args]

Options:
  -v, --verbose                  verbose mode
  -l logfile, --logfile=logfile  path to logfile
  -h, --help                     show this help message and exit
  --version                      show the program version and exit

Commands:
  install    install given package (aliases: inst, instbis)
  uninstall  uninstall given package

--TEST--
Test for Console_CommandLine::addCommand() method.
--ARGS--
cmd1 --help 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->renderer->line_width = 30;
$parser->addCommand('cmd1', array(
    'description' => '1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30'
));
$parser->parse();

?>
--EXPECTF--

1 2 3 4 5 6 7 8 9 10 11 12 13
14 15 16 17 18 19 20 21 22 23
24 25 26 27 28 29 30

Usage:
  %sconsole_commandline_addcommand_2.php
  [options] cmd1 [options]

Options:
  -h, --help  show this help
              message and exit

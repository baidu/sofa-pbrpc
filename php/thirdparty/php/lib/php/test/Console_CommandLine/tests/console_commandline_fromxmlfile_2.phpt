--TEST--
Test for Console_CommandLine::fromXmlFile() method.
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
unzip --help 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = Console_CommandLine::fromXmlFile(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test.xml');
$parser->parse();

?>
--EXPECTF--
unzip given file in the destination dir

Usage:
  test [options] unzip [options] outputdir zipfile

Options:
  -h, --help  show this help message and exit

Arguments:
  outputdir  destination directory
  zipfile    path to the zip file to unzip

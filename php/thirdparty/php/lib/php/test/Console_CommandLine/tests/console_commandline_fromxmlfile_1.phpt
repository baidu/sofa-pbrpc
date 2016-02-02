--TEST--
Test for Console_CommandLine::fromXmlFile() method.
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
zip --help 2>&1
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = Console_CommandLine::fromXmlFile(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'test.xml');
$parser->parse();

?>
--EXPECTF--
zip given files in the destination file

Usage:
  test [options] zip [options] files1 files2 ... zipfile

Options:
  -h, --help  show this help message and exit

Arguments:
  files    a list of files to zip together
  zipfile  path to the zip file to generate

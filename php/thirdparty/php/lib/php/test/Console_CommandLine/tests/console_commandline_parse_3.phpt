--TEST--
Test for Console_CommandLine::parse() method (various options).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
-tfsfoo --int=3 --flo 4.0 -cccc --callback=somestring -a foo bar baz foo bar
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser1();
$result = $parser->parse();
var_dump($result->options);
var_dump($result->args);

?>
--EXPECT--
array(11) {
  ["true"]=>
  bool(true)
  ["false"]=>
  bool(false)
  ["int"]=>
  int(3)
  ["float"]=>
  float(4)
  ["string"]=>
  string(3) "foo"
  ["counter"]=>
  int(4)
  ["callback"]=>
  string(20) "foo__fbzrfgevat__bar"
  ["array"]=>
  array(3) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
    [2]=>
    string(3) "baz"
  }
  ["password"]=>
  NULL
  ["help"]=>
  NULL
  ["version"]=>
  NULL
}
array(2) {
  ["simple"]=>
  string(3) "foo"
  ["multiple"]=>
  array(1) {
    [0]=>
    string(3) "bar"
  }
}

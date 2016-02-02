--TEST--
Test for Console_CommandLine::addOption() method.
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addOption('opt1', array(
    'short_name' => '-a'
));
$parser->addOption('opt2', array(
    'short_name' => '-b',
    'long_name' => '--foo',
    'description' => 'description of opt2',
    'action' => 'StoreInt',
    'help_name' => 'bar',
    'choices' => array(1, 2, 3),
    'add_list_option' => true,
    'default' => 2
));
$opt3 = new Console_CommandLine_Option('opt3', array(
    'long_name' => '--bar',
    'description' => 'description of opt3',
));
$parser->addOption($opt3);

var_dump($parser->options);

?>
--EXPECTF--
array(4) {
  ["opt1"]=>
  object(Console_CommandLine_Option)#5 (14) {
    ["short_name"]=>
    string(2) "-a"
    ["long_name"]=>
    NULL
    ["action"]=>
    string(11) "StoreString"
    ["choices"]=>
    array(0) {
    }
    ["callback"]=>
    NULL
    ["action_params"]=>
    array(0) {
    }
    ["argument_optional"]=>
    bool(false)
    ["add_list_option"]=>
    bool(false)
    [%s]=>
    NULL
    ["name"]=>
    string(4) "opt1"
    ["help_name"]=>
    string(4) "opt1"
    ["description"]=>
    NULL
    ["default"]=>
    NULL
    ["messages"]=>
    array(0) {
    }
  }
  ["opt2"]=>
  object(Console_CommandLine_Option)#6 (14) {
    ["short_name"]=>
    string(2) "-b"
    ["long_name"]=>
    string(5) "--foo"
    ["action"]=>
    string(8) "StoreInt"
    ["choices"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
    ["callback"]=>
    NULL
    ["action_params"]=>
    array(0) {
    }
    ["argument_optional"]=>
    bool(false)
    ["add_list_option"]=>
    bool(true)
    [%s]=>
    NULL
    ["name"]=>
    string(4) "opt2"
    ["help_name"]=>
    string(3) "bar"
    ["description"]=>
    string(19) "description of opt2"
    ["default"]=>
    int(2)
    ["messages"]=>
    array(0) {
    }
  }
  ["list_opt2"]=>
  object(Console_CommandLine_Option)#7 (14) {
    ["short_name"]=>
    NULL
    ["long_name"]=>
    string(11) "--list-opt2"
    ["action"]=>
    string(4) "List"
    ["choices"]=>
    array(0) {
    }
    ["callback"]=>
    NULL
    ["action_params"]=>
    array(1) {
      ["list"]=>
      array(3) {
        [0]=>
        int(1)
        [1]=>
        int(2)
        [2]=>
        int(3)
      }
    }
    ["argument_optional"]=>
    bool(false)
    ["add_list_option"]=>
    bool(false)
    [%s]=>
    NULL
    ["name"]=>
    string(9) "list_opt2"
    ["help_name"]=>
    string(9) "list_opt2"
    ["description"]=>
    string(35) "lists valid choices for option opt2"
    ["default"]=>
    NULL
    ["messages"]=>
    array(0) {
    }
  }
  ["opt3"]=>
  object(Console_CommandLine_Option)#8 (14) {
    ["short_name"]=>
    NULL
    ["long_name"]=>
    string(5) "--bar"
    ["action"]=>
    string(11) "StoreString"
    ["choices"]=>
    array(0) {
    }
    ["callback"]=>
    NULL
    ["action_params"]=>
    array(0) {
    }
    ["argument_optional"]=>
    bool(false)
    ["add_list_option"]=>
    bool(false)
    [%s]=>
    NULL
    ["name"]=>
    string(4) "opt3"
    ["help_name"]=>
    string(4) "opt3"
    ["description"]=>
    string(19) "description of opt3"
    ["default"]=>
    NULL
    ["messages"]=>
    array(0) {
    }
  }
}

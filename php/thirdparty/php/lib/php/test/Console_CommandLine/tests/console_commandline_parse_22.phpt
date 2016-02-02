--TEST--
Test for Console_CommandLine::parse() method (--help with renderer options).
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--ARGS--
--list
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = new Console_CommandLine();
$parser->addOption('list', array(
    'long_name'     => '--list',
    'action'        => 'List',
    'action_params' => array(
        'list'      => array('foo', 'bar', 'baz'),
        'message'   => 'foobarbaz---',
        'delimiter' => '|',
        'post'      => '---foobarbaz',
    ),
));
$parser->parse();

?>
--EXPECT--
foobarbaz---foo|bar|baz---foobarbaz

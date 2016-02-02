<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * This file is part of the PEAR Console_CommandLine package.
 *
 * A simple example demonstrating the use of subcommands.
 *
 * PHP version 5
 *
 * LICENSE: This source file is subject to the MIT license that is available
 * through the world-wide-web at the following URI:
 * http://opensource.org/licenses/mit-license.php
 *
 * @category  Console 
 * @package   Console_CommandLine
 * @author    David JEAN LOUIS <izimobil@gmail.com>
 * @copyright 2007 David JEAN LOUIS
 * @license   http://opensource.org/licenses/mit-license.php MIT License 
 * @version   CVS: $Id$
 * @link      http://pear.php.net/package/Console_CommandLine
 * @since     File available since release 0.1.0
 */

// Include the Console_CommandLine package.
require_once 'Console/CommandLine.php';

// create the parser
$parser = new Console_CommandLine(array(
    'description' => 'A great program that can foo and bar !',
    'version'     => '1.0.0'
));

// add a global option to make the program verbose
$parser->addOption('verbose', array(
    'short_name'  => '-v',
    'long_name'   => '--verbose',
    'action'      => 'StoreTrue',
    'description' => 'turn on verbose output'
));

// add the foo subcommand
$foo_cmd = $parser->addCommand('foo', array(
    'description' => 'output the given string with a foo prefix'
));
$foo_cmd->addOption('reverse', array(
    'short_name'  => '-r',
    'long_name'   => '--reverse',
    'action'      => 'StoreTrue',
    'description' => 'reverse the given string before echoing it'
));
$foo_cmd->addArgument('text', array(
    'description' => 'the text to output'
));

// add the bar subcommand with a "baz" alias
$bar_cmd = $parser->addCommand('bar', array(
    'description' => 'output the given string with a bar prefix',
    'aliases'     => array('baz'),
));
$bar_cmd->addOption('reverse', array(
    'short_name'  => '-r',
    'long_name'   => '--reverse',
    'action'      => 'StoreTrue',
    'description' => 'reverse the given string before echoing it'
));
$bar_cmd->addArgument('text', array(
    'description' => 'the text to output'
));

// run the parser
try {
    $result = $parser->parse();
    if ($result->command_name) {
        $st = $result->command->options['reverse'] 
            ? strrev($result->command->args['text'])
            : $result->command->args['text'];
        if ($result->command_name == 'foo') { 
            echo "Foo says: $st\n";
        } else if ($result->command_name == 'bar') {
            echo "Bar says: $st\n";
        }
    }
} catch (Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>

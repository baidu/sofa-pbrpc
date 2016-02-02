<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * This file is part of the PEAR Console_CommandLine package.
 *
 * A simple example demonstrating the basic features of the Console_CommandLine
 * package.
 * In this example we create a program that simply zip a set of files.
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
    'description' => 'zip given files using the php zip module.',
    'version'     => '1.0.0'
));

// add an option to make the program verbose
$parser->addOption('verbose', array(
    'short_name'  => '-v',
    'long_name'   => '--verbose',
    'action'      => 'StoreTrue',
    'description' => 'turn on verbose output'
));

// add an option to delete original files after zipping
$parser->addOption('delete', array(
    'short_name'  => '-d',
    'long_name'   => '--delete',
    'action'      => 'StoreString',
    'description' => 'delete original files after zip operation',
    'choices'     => array('foo', 'bar'),
    'add_list_option' => true
));

// add the files argument, the user can specify one or several files
$parser->addArgument('files', array(
    'multiple' => true,
    'description' => 'list of files to zip separated by spaces'
));

// add the zip file name argument
$parser->addArgument('zipfile', array('description' => 'zip file name'));

// run the parser
try {
    $result = $parser->parse();
    // write your program here...
    print_r($result->options);
    print_r($result->args);
} catch (Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>

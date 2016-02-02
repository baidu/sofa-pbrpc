<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * This file is part of the PEAR Console_CommandLine package.
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

if (php_sapi_name() != 'cli') {
    // tests with php-cgi need this
    ini_set('include_path', ini_get('include_path') . PATH_SEPARATOR 
        . dirname(__FILE__) . '/../');
}

/**
 * Required classes
 */
require_once 'Console/CommandLine.php';
require_once 'Console/CommandLine/Renderer.php';
require_once 'Console/CommandLine/Outputter.php';
require_once 'Console/CommandLine/MessageProvider.php';

// rot13Callback() {{{

/**
 * A dummy callback for tests purposes.
 * 
 * @param mixed  $value  value provided by the user
 * @param object $option the option instance
 * @param object $result the result instance
 * @param object $parser the parser instance
 * @param array  $params optional params array
 * 
 * @return string
 */
function rot13Callback($value, $option, $result, $parser, $params=array())
{
    $ret = '';
    if (isset($params['prefix'])) {
        $ret .= $params['prefix'] . '__';
    }
    $ret .= str_rot13($value);
    if (isset($params['suffix'])) {
        $ret .= '__' . $params['suffix'];
    }
    return $ret;
}

// }}}
// buildParser1() {{{

/**
 * Build a parser instance and return it.
 *
 * @return object Console_CommandLine instance
 */
function buildParser1()
{
    $parser              = new Console_CommandLine();
    $parser->name        = 'some_program';
    $parser->version     = '0.1.0';
    $parser->description = 'Description of our parser goes here...';

    // add options
    $parser->addOption('true', array(
        'short_name'  => '-t',
        'long_name'   => '--true',
        'action'      => 'StoreTrue',
        'description' => 'test the StoreTrue action'
    ));
    $parser->addOption('false', array(
        'short_name'  => '-f',
        'long_name'   => '--false',
        'action'      => 'StoreFalse',
        'description' => 'test the StoreFalse action'
    ));
    $parser->addOption('int', array(
        'long_name'   => '--int',
        'action'      => 'StoreInt',
        'description' => 'test the StoreInt action',
        'help_name'   => 'INT',
        'default'     => 1
    ));
    $parser->addOption('float', array(
        'long_name'   => '--float',
        'action'      => 'StoreFloat',
        'description' => 'test the StoreFloat action',
        'help_name'   => 'FLOAT',
        'default'     => 1.0
    ));
    $parser->addOption('string', array(
        'short_name'  => '-s',
        'long_name'   => '--string',
        'action'      => 'StoreString',
        'description' => 'test the StoreString action',
        'help_name'   => 'STRING',
        'choices'     => array('foo', 'bar', 'baz')
    ));
    $parser->addOption('counter', array(
        'short_name'  => '-c',
        'long_name'   => '--counter',
        'action'      => 'Counter',
        'description' => 'test the Counter action'
    ));
    $parser->addOption('callback', array(
        'long_name'     => '--callback',
        'action'        => 'Callback',
        'description'   => 'test the Callback action',
        'callback'      => 'rot13Callback',
        'action_params' => array('prefix' => 'foo', 'suffix' => 'bar')
    ));
    $parser->addOption('array', array(
        'short_name'  => '-a',
        'long_name'   => '--array',
        'default'     => array('spam', 'egg'),
        'action'      => 'StoreArray',
        'help_name'   => 'ARRAY',
        'description' => 'test the StoreArray action'
    ));
    $parser->addOption('password', array(
        'short_name'  => '-p',
        'long_name'   => '--password',
        'action'      => 'Password',
        'description' => 'test the Password action'
    ));
    $parser->addArgument('simple', array(
        'description' => 'test a simple argument'
    ));
    $parser->addArgument('multiple', array(
        'description' => 'test a multiple argument',
        'multiple'    => true,
        'optional'    => true
    ));
    return $parser;
}

// }}}
// buildParser2() {{{

/**
 * Build a parser instance and return it.
 *
 * @return object Console_CommandLine instance
 */
function buildParser2()
{
    $parser              = new Console_CommandLine();
    $parser->name        = 'some_program';
    $parser->version     = '0.1.0';
    $parser->description = 'Description of our parser goes here...';

    // add general options
    $parser->addOption('verbose', array(
        'short_name'  => '-v',
        'long_name'   => '--verbose',
        'action'      => 'StoreTrue',
        'description' => 'verbose mode'
    ));
    $parser->addOption('logfile', array(
        'short_name'  => '-l',
        'long_name'   => '--logfile',
        'action'      => 'StoreString',
        'description' => 'path to logfile'
    ));
 
    // install subcommand
    $cmd1 = $parser->addCommand('install', array(
        'description' => 'install given package',
        'aliases'     => array('inst', 'instbis'),
    ));
    $cmd1->addOption('force', array(
        'short_name'  => '-f',
        'long_name'   => '--force',
        'action'      => 'StoreTrue',
        'description' => 'force installation'
    ));
    $cmd1->addArgument('package', array(
        'description' => 'package to install'
    ));

    // uninstall subcommand
    $cmd2 = $parser->addCommand('uninstall', array(
        'description' => 'uninstall given package'
    ));
    $cmd2->addArgument('package', array(
        'description' => 'package to uninstall'
    ));
    return $parser;
}

// }}}
// buildParser3() {{{

/**
 * Build a parser instance and return it.
 *
 * @return object Console_CommandLine instance
 */
function buildParser3()
{
    $parser              = new Console_CommandLine();
    $parser->name        = 'some_program';
    $parser->version     = '0.1.0';
    $parser->description = 'Description of our parser goes here...';
    // we force options default values
    $parser->force_options_defaults = true;

    // add options
    $parser->addOption('true', array(
        'short_name'  => '-t',
        'long_name'   => '--true',
        'action'      => 'StoreTrue',
        'description' => 'test the StoreTrue action',
    ));
    $parser->addOption('false', array(
        'short_name'  => '-f',
        'long_name'   => '--false',
        'action'      => 'StoreFalse',
        'description' => 'test the StoreFalse action',
    ));
    $parser->addOption('int', array(
        'long_name'   => '--int',
        'action'      => 'StoreInt',
        'description' => 'test the StoreInt action',
        'help_name'   => 'INT',
    ));
    $parser->addOption('float', array(
        'long_name'   => '--float',
        'action'      => 'StoreFloat',
        'description' => 'test the StoreFloat action',
        'help_name'   => 'FLOAT',
    ));
    $parser->addOption('string', array(
        'short_name'  => '-s',
        'long_name'   => '--string',
        'action'      => 'StoreString',
        'description' => 'test the StoreString action',
        'help_name'   => 'STRING',
        'choices'     => array('foo', 'bar', 'baz')
    ));
    $parser->addOption('counter', array(
        'short_name'  => '-c',
        'long_name'   => '--counter',
        'action'      => 'Counter',
        'description' => 'test the Counter action'
    ));
    $parser->addOption('callback', array(
        'long_name'     => '--callback',
        'action'        => 'Callback',
        'description'   => 'test the Callback action',
        'callback'      => 'rot13Callback',
        'action_params' => array('prefix' => 'foo', 'suffix' => 'bar')
    ));
    $parser->addOption('array', array(
        'short_name'  => '-a',
        'long_name'   => '--array',
        'action'      => 'StoreArray',
        'help_name'   => 'ARRAY',
        'description' => 'test the StoreArray action'
    ));
    $parser->addOption('password', array(
        'short_name'  => '-p',
        'long_name'   => '--password',
        'action'      => 'Password',
        'description' => 'test the Password action'
    ));
    return $parser;
}

// }}}
// {{{ buildParser4()

/**
 * Build a parser instance and return it.
 *
 * For testing custom messages.
 *
 * @return object Console_CommandLine instance
 */
function buildParser4()
{
    $parser = new Console_CommandLine(array(
        'messages' => array(
            'INVALID_SUBCOMMAND' => 'Only "upgrade" is supported.',
        ),
    ));
    $parser->name        = 'some_program';
    $parser->version     = '0.1.0';
    $parser->description = 'Description of our parser goes here...';

    // some subcommand
    $cmd1 = $parser->addCommand('upgrade', array(
        'description' => 'upgrade given package',
        'aliases'     => array('up'),
        'messages'    => array(
            'ARGUMENT_REQUIRED'       => 'Package name is required.',
            'OPTION_VALUE_REQUIRED'   => 'Option requires value.',
            'OPTION_VALUE_UNEXPECTED' => 'Option should not have a value.',
            'OPTION_UNKNOWN'          => 'Mysterious option encountered.',
        ),
    ));
    // add option
    $cmd1->addOption('state', array(
        'short_name'  => '-s',
        'long_name'   => '--state',
        'action'      => 'StoreString',
        'choices'     => array('stable', 'beta'),
        'description' => 'accepted package states',
        'messages'    => array(
            'OPTION_VALUE_NOT_VALID' => 'Valid states are "stable" and "beta".',
        ),
    ));
    // add another option
    $cmd1->addOption('dry_run', array(
        'short_name'  => '-d',
        'long_name'   => '--dry-run',
        'action'      => 'StoreTrue',
        'description' => 'dry run',
    ));
    // add argument
    $cmd1->addArgument('package', array(
        'description' => 'package to upgrade'
    ));

    return $parser;
}

// }}}
// CustomRenderer() {{{

/**
 * Some custom renderer for tests purposes.
 *
 * @category  Console
 * @package   Console_CommandLine
 * @author    David JEAN LOUIS <izimobil@gmail.com>
 * @copyright 2007 David JEAN LOUIS
 * @license   http://opensource.org/licenses/mit-license.php MIT License 
 * @version   Release: 1.2.0
 * @link      http://pear.php.net/package/Console_CommandLine
 * @since     File available since release 0.1.0
 */
class CustomRenderer implements Console_CommandLine_Renderer
{
    // usage() {{{

    /**
     * Return the full usage message
     *
     * @return string the usage message
     * @access public
     */
    public function usage()
    {
        return __METHOD__ . '()';
    }
    // }}}
    // error() {{{

    /**
     * Return a formatted error message
     *
     * @param string $error the error message to format
     *
     * @return string the error string
     * @access public
     */
    public function error($error)
    {
        return __METHOD__ . "($error)";
    }

    // }}}
    // version() {{{

    /**
     * Return the program version string
     *
     * @return string the version string
     * @access public
     */
    public function version()
    {
        return __METHOD__ . '()';
    }

    // }}}
}

// }}}
// CustomOutputter() {{{

/**
 * Some custom outputter for tests purposes.
 *
 * @category  Console
 * @package   Console_CommandLine
 * @author    David JEAN LOUIS <izimobil@gmail.com>
 * @copyright 2007 David JEAN LOUIS
 * @license   http://opensource.org/licenses/mit-license.php MIT License 
 * @version   Release: 1.2.0
 * @link      http://pear.php.net/package/Console_CommandLine
 * @since     File available since release 0.1.0
 */
class CustomOutputter implements Console_CommandLine_Outputter
{
    // stdout() {{{

    /**
     * Called for stdout messages.
     *
     * @param string $msg the message to output
     *
     * @return void
     * @access public
     */
    public function stdout($msg)
    {
        echo "STDOUT >> $msg\n";
    }

    // }}}
    // stderr() {{{

    /**
     * Called for stderr messages.
     *
     * @param string $msg the message to output
     *
     * @return void
     * @access public
     */
    public function stderr($msg)
    {
        echo "STDERR >> $msg\n";
    }

    // }}}
}

// }}}
// CustomMessageProvider() {{{

/**
 * Some custom message provider for tests purposes.
 *
 * @category  Console
 * @package   Console_CommandLine
 * @author    David JEAN LOUIS <izimobil@gmail.com>
 * @copyright 2007 David JEAN LOUIS
 * @license   http://opensource.org/licenses/mit-license.php MIT License 
 * @version   Release: 1.2.0
 * @link      http://pear.php.net/package/Console_CommandLine
 * @since     File available since release 0.1.0
 */
class CustomMessageProvider implements Console_CommandLine_MessageProvider
{
    // get() {{{

    /**
     * Retrieve the given string identifier corresponding message.
     *
     * @param string $code the string identifier of the message
     * @param array  $vars an array of template variables
     *
     * @return string
     * @access public
     */
    public function get($code, $vars = array())
    {
        return $code;
    }

    // }}}
}

// }}}

?>

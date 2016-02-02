<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * This file is part of the PEAR Console_CommandLine package.
 *
 * A simple example demonstrating the use of choices argument validation.
 *
 * PHP version 5
 *
 * @category  Console
 * @package   Console_CommandLine
 * @author    Laurent Laville <pear@laurent-laville.org>
 * @link      http://pear.php.net/package/Console_CommandLine
 */

// Include the Console_CommandLine package.
require_once 'Console/CommandLine.php';

// create the parser
$xmlfile = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'ex5.xml';
$parser  = Console_CommandLine::fromXmlFile($xmlfile);

// run the parser
try {
    $result = $parser->parse();
    if ($result->command_name) {
        $st = implode(', ', $result->command->args['item']);
        echo "List says: $st\n";
    }
} catch (Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>

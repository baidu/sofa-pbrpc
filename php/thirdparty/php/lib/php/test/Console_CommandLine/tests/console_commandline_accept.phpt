--TEST--
Test for Console_CommandLine::accept() method.
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

$parser = buildParser1();
try {
    // custom renderer
    $parser->accept(new CustomRenderer());
    echo get_class($parser->renderer) . "\n";
    // outputter
    $parser->accept(new CustomOutputter());
    echo get_class($parser->outputter) . "\n";
    $parser->accept(new CustomMessageProvider());
    echo get_class($parser->message_provider) . "\n";
    $parser->accept(new stdclass());
} catch (Console_CommandLine_Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>
--EXPECT--
CustomRenderer
CustomOutputter
CustomMessageProvider
STDERR >> CustomRenderer::error(INVALID_CUSTOM_INSTANCE)

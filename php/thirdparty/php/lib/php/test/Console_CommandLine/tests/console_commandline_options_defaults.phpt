--TEST--
Test for Console_CommandLine options defaults.
--SKIPIF--
<?php if(php_sapi_name()!='cli') echo 'skip'; ?>
--FILE--
<?php

require_once dirname(__FILE__) . DIRECTORY_SEPARATOR . 'tests.inc.php';

try {
    $parser = buildParser3();
    $parser->force_options_defaults = true;
    $result = $parser->parse();
    foreach ($result->options as $k => $v) {
        echo $k . ":"; var_dump($v);
    }
} catch (Console_CommandLine_Exception $exc) {
    $parser->displayError($exc->getMessage());
}

?>
--EXPECT--
true:bool(false)
false:bool(true)
int:int(0)
float:float(0)
string:NULL
counter:int(0)
callback:NULL
array:array(0) {
}
password:NULL
help:NULL
version:NULL

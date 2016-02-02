<?php
/**
 * Auto generated from test.proto at 2016-02-02 21:48:42
 *
 * sofa.pbrpc.test package
 */

/**
 * TestPair message
 */
class TestPair extends ProtobufMessage
{
    /* Field index constants */
    const ID = 1;
    const VALUE = 2;

    /* @var string class type */
    protected static $class_type = "TestPair";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::ID => array(
            'name' => 'id',
            'required' => false,
            'type' => 7,
        ),
        self::VALUE => array(
            'name' => 'value',
            'required' => false,
            'type' => 7,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function copy($obj)
    {
        $this->values = $obj->values;
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::ID] = null;
        $this->values[self::VALUE] = null;
    }

    /**
     * Returns class type
     *
     * @return string
     */
    public function class_type()
    {
        return self::$class_type;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'id' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_id($value)
    {
        return $this->set(self::ID, $value);
    }

    /**
     * Returns value of 'id' property
     *
     * @return string
     */
    public function get_id()
    {
        return $this->get(self::ID);
    }

    /**
     * Sets value of 'value' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_value($value)
    {
        return $this->set(self::VALUE, $value);
    }

    /**
     * Returns value of 'value' property
     *
     * @return string
     */
    public function get_value()
    {
        return $this->get(self::VALUE);
    }
}

/**
 * Result message
 */
class Result extends ProtobufMessage
{
    /* Field index constants */
    const INDEX = 1;
    const DATA = 2;

    /* @var string class type */
    protected static $class_type = "Result";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::INDEX => array(
            'name' => 'index',
            'required' => false,
            'type' => 5,
        ),
        self::DATA => array(
            'name' => 'data',
            'required' => false,
            'type' => 7,
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function copy($obj)
    {
        $this->values = $obj->values;
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::INDEX] = null;
        $this->values[self::DATA] = null;
    }

    /**
     * Returns class type
     *
     * @return string
     */
    public function class_type()
    {
        return self::$class_type;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'index' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_index($value)
    {
        return $this->set(self::INDEX, $value);
    }

    /**
     * Returns value of 'index' property
     *
     * @return int
     */
    public function get_index()
    {
        return $this->get(self::INDEX);
    }

    /**
     * Sets value of 'data' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_data($value)
    {
        return $this->set(self::DATA, $value);
    }

    /**
     * Returns value of 'data' property
     *
     * @return string
     */
    public function get_data()
    {
        return $this->get(self::DATA);
    }
}

/**
 * Request message
 */
class Request extends ProtobufMessage
{
    /* Field index constants */
    const DEBUG_MESSAGE = 1;
    const PAIR = 2;

    /* @var string class type */
    protected static $class_type = "Request";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::DEBUG_MESSAGE => array(
            'name' => 'debug_message',
            'required' => false,
            'type' => 7,
        ),
        self::PAIR => array(
            'name' => 'pair',
            'repeated' => true,
            'type' => 'TestPair'
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function copy($obj)
    {
        $this->values = $obj->values;
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::DEBUG_MESSAGE] = null;
        $this->values[self::PAIR] = array();
    }

    /**
     * Returns class type
     *
     * @return string
     */
    public function class_type()
    {
        return self::$class_type;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'debug_message' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_debug_message($value)
    {
        return $this->set(self::DEBUG_MESSAGE, $value);
    }

    /**
     * Returns value of 'debug_message' property
     *
     * @return string
     */
    public function get_debug_message()
    {
        return $this->get(self::DEBUG_MESSAGE);
    }

    /**
     * Sets value of 'pair' property
     *
     * @param TestPair $value Property value
     *
     * @return null
     */
    public function set_pair($value)
    {
        return $this->set(self::PAIR, $value);
    }

    /**
     * Appends value to 'pair' list
     *
     * @param TestPair $value Value to append
     *
     * @return null
     */
    public function append_pair(TestPair $value)
    {
        return $this->append(self::PAIR, $value);
    }

    /**
     * Clears 'pair' list
     *
     * @return null
     */
    public function clear_pair()
    {
        return $this->clear(self::PAIR);
    }

    /**
     * Returns 'pair' list
     *
     * @return TestPair[]
     */
    public function get_pair()
    {
        return $this->get(self::PAIR);
    }

    /**
     * Returns 'pair' iterator
     *
     * @return ArrayIterator
     */
    public function get_pairIterator()
    {
        return new ArrayIterator($this->get(self::PAIR));
    }

    /**
     * Returns element from 'pair' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return TestPair
     */
    public function get_pairAt($offset)
    {
        return $this->get(self::PAIR, $offset);
    }

    /**
     * Returns element from 'pair' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return TestPair
     */
    public function get_pairObjectAt($offset)
    {
        $value = $this->get(self::PAIR, $offset);
        $instance = new TestPair();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'pair' list
     *
     * @return int
     */
    public function get_pairCount()
    {
        return $this->count(self::PAIR);
    }
}

/**
 * Response message
 */
class Response extends ProtobufMessage
{
    /* Field index constants */
    const DEBUG_MESSAGE = 1;
    const RESULT = 2;

    /* @var string class type */
    protected static $class_type = "Response";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::DEBUG_MESSAGE => array(
            'name' => 'debug_message',
            'required' => false,
            'type' => 7,
        ),
        self::RESULT => array(
            'name' => 'result',
            'repeated' => true,
            'type' => 'Result'
        ),
    );

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function __construct()
    {
        $this->reset();
    }

    /**
     * Constructs new message container and clears its internal state
     *
     * @return null
     */
    public function copy($obj)
    {
        $this->values = $obj->values;
    }

    /**
     * Clears message values and sets default ones
     *
     * @return null
     */
    public function reset()
    {
        $this->values[self::DEBUG_MESSAGE] = null;
        $this->values[self::RESULT] = array();
    }

    /**
     * Returns class type
     *
     * @return string
     */
    public function class_type()
    {
        return self::$class_type;
    }

    /**
     * Returns field descriptors
     *
     * @return array
     */
    public function fields()
    {
        return self::$fields;
    }

    /**
     * Sets value of 'debug_message' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_debug_message($value)
    {
        return $this->set(self::DEBUG_MESSAGE, $value);
    }

    /**
     * Returns value of 'debug_message' property
     *
     * @return string
     */
    public function get_debug_message()
    {
        return $this->get(self::DEBUG_MESSAGE);
    }

    /**
     * Sets value of 'result' property
     *
     * @param Result $value Property value
     *
     * @return null
     */
    public function set_result($value)
    {
        return $this->set(self::RESULT, $value);
    }

    /**
     * Appends value to 'result' list
     *
     * @param Result $value Value to append
     *
     * @return null
     */
    public function append_result(Result $value)
    {
        return $this->append(self::RESULT, $value);
    }

    /**
     * Clears 'result' list
     *
     * @return null
     */
    public function clear_result()
    {
        return $this->clear(self::RESULT);
    }

    /**
     * Returns 'result' list
     *
     * @return Result[]
     */
    public function get_result()
    {
        return $this->get(self::RESULT);
    }

    /**
     * Returns 'result' iterator
     *
     * @return ArrayIterator
     */
    public function get_resultIterator()
    {
        return new ArrayIterator($this->get(self::RESULT));
    }

    /**
     * Returns element from 'result' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return Result
     */
    public function get_resultAt($offset)
    {
        return $this->get(self::RESULT, $offset);
    }

    /**
     * Returns element from 'result' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return Result
     */
    public function get_resultObjectAt($offset)
    {
        $value = $this->get(self::RESULT, $offset);
        $instance = new Result();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'result' list
     *
     * @return int
     */
    public function get_resultCount()
    {
        return $this->count(self::RESULT);
    }
}
class TestService extends PhpRpcServiceStub
{
    private function Registertest_func()
    {

        $TestPair_instance = New TestPair();


        $Request_instance = New Request();

        $Request_instance->append_pair($TestPair_instance);

        $Result_instance = New Result();


        $Response_instance = New Response();

        $Response_instance->append_result($Result_instance);

        $this->RegisterMethod("test_func", $Request_instance, $Response_instance);
    }

    function __construct($server_address)
    {
        $this->InitService($server_address, "sofa.pbrpc.test", "TestService");
        $this->Registertest_func();
        $this->InitMethods();
    }

    public function test_func($request, $response, $closure)
    {
        $this->CallMethod("test_func", $request, $response, $closure);
    }

    public function Failed()
    {
        return $this->GetFailed();
    }

    public function ErrorText()
    {
        return $this->GetErrorText();
    }

}

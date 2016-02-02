<?php
/**
 * Auto generated from test.proto at 2016-02-02 11:04:43
 *
 * sofa.pbrpc.vui package
 */

/**
 * uap_result_item_t message
 */
class uap_result_item_t extends ProtobufMessage
{
    /* Field index constants */
    const ID = 1;
    const VALUE = 2;

    /* @var string class type */
    protected static $class_type = "uap_result_item_t";

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
 * OutNodeInfo message
 */
class OutNodeInfo extends ProtobufMessage
{
    /* Field index constants */
    const INDEX = 1;
    const GSAMPLE_LOG = 2;
    const CLICK_WEIGHT = 3;
    const CLICK_WEIGHT_MERGE = 4;
    const CLICK_WEIGHT_MERGED_TIME = 5;
    const CLICK_WEIGHT_ORIG = 6;
    const CLICK_TIME_WEIGHT = 7;
    const CLICK_TIME_RATIO = 8;
    const CLICK_TIME_RATIO_A = 9;
    const CLICK_TIME_RATIO_B = 10;
    const CLICK_TIME_LEVEL = 11;
    const CLICK_ORIG_ALA_POS = 12;
    const CLICK_ORIG_POS = 13;
    const URL_DISPLAY = 14;
    const URL_EXAM = 15;
    const URL_CLICK = 16;
    const URL_SATISFY = 17;
    const URL_SKIP = 18;
    const URL_SATISFY_DEGREE = 19;
    const URL_CLICK_NEED = 20;
    const URL_CLICK_RATIO = 21;
    const URL_SATISFY_RATIO = 22;
    const CLICK_AUTO_HOLD_ORIG = 23;
    const CLICK_CONF_FILTER = 24;
    const CLICK_RTSE_HOLD = 25;
    const CSTRA = 26;

    /* @var string class type */
    protected static $class_type = "OutNodeInfo";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::INDEX => array(
            'name' => 'index',
            'required' => false,
            'type' => 5,
        ),
        self::GSAMPLE_LOG => array(
            'name' => 'gsample_log',
            'required' => false,
            'type' => 7,
        ),
        self::CLICK_WEIGHT => array(
            'name' => 'click_weight',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_WEIGHT_MERGE => array(
            'name' => 'click_weight_merge',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_WEIGHT_MERGED_TIME => array(
            'name' => 'click_weight_merged_time',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_WEIGHT_ORIG => array(
            'name' => 'click_weight_orig',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_TIME_WEIGHT => array(
            'name' => 'click_time_weight',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_TIME_RATIO => array(
            'name' => 'click_time_ratio',
            'required' => false,
            'type' => 4,
        ),
        self::CLICK_TIME_RATIO_A => array(
            'name' => 'click_time_ratio_a',
            'required' => false,
            'type' => 4,
        ),
        self::CLICK_TIME_RATIO_B => array(
            'name' => 'click_time_ratio_b',
            'required' => false,
            'type' => 4,
        ),
        self::CLICK_TIME_LEVEL => array(
            'name' => 'click_time_level',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_ORIG_ALA_POS => array(
            'name' => 'click_orig_ala_pos',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_ORIG_POS => array(
            'name' => 'click_orig_pos',
            'required' => false,
            'type' => 5,
        ),
        self::URL_DISPLAY => array(
            'name' => 'url_display',
            'required' => false,
            'type' => 5,
        ),
        self::URL_EXAM => array(
            'name' => 'url_exam',
            'required' => false,
            'type' => 5,
        ),
        self::URL_CLICK => array(
            'name' => 'url_click',
            'required' => false,
            'type' => 5,
        ),
        self::URL_SATISFY => array(
            'name' => 'url_satisfy',
            'required' => false,
            'type' => 5,
        ),
        self::URL_SKIP => array(
            'name' => 'url_skip',
            'required' => false,
            'type' => 5,
        ),
        self::URL_SATISFY_DEGREE => array(
            'name' => 'url_satisfy_degree',
            'required' => false,
            'type' => 5,
        ),
        self::URL_CLICK_NEED => array(
            'name' => 'url_click_need',
            'required' => false,
            'type' => 5,
        ),
        self::URL_CLICK_RATIO => array(
            'name' => 'url_click_ratio',
            'required' => false,
            'type' => 5,
        ),
        self::URL_SATISFY_RATIO => array(
            'name' => 'url_satisfy_ratio',
            'required' => false,
            'type' => 5,
        ),
        self::CLICK_AUTO_HOLD_ORIG => array(
            'name' => 'click_auto_hold_orig',
            'required' => false,
            'type' => 8,
        ),
        self::CLICK_CONF_FILTER => array(
            'name' => 'click_conf_filter',
            'required' => false,
            'type' => 8,
        ),
        self::CLICK_RTSE_HOLD => array(
            'name' => 'click_rtse_hold',
            'required' => false,
            'type' => 8,
        ),
        self::CSTRA => array(
            'name' => 'cstra',
            'required' => false,
            'type' => 5,
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
        $this->values[self::GSAMPLE_LOG] = null;
        $this->values[self::CLICK_WEIGHT] = null;
        $this->values[self::CLICK_WEIGHT_MERGE] = null;
        $this->values[self::CLICK_WEIGHT_MERGED_TIME] = null;
        $this->values[self::CLICK_WEIGHT_ORIG] = null;
        $this->values[self::CLICK_TIME_WEIGHT] = null;
        $this->values[self::CLICK_TIME_RATIO] = null;
        $this->values[self::CLICK_TIME_RATIO_A] = null;
        $this->values[self::CLICK_TIME_RATIO_B] = null;
        $this->values[self::CLICK_TIME_LEVEL] = null;
        $this->values[self::CLICK_ORIG_ALA_POS] = null;
        $this->values[self::CLICK_ORIG_POS] = null;
        $this->values[self::URL_DISPLAY] = null;
        $this->values[self::URL_EXAM] = null;
        $this->values[self::URL_CLICK] = null;
        $this->values[self::URL_SATISFY] = null;
        $this->values[self::URL_SKIP] = null;
        $this->values[self::URL_SATISFY_DEGREE] = null;
        $this->values[self::URL_CLICK_NEED] = null;
        $this->values[self::URL_CLICK_RATIO] = null;
        $this->values[self::URL_SATISFY_RATIO] = null;
        $this->values[self::CLICK_AUTO_HOLD_ORIG] = null;
        $this->values[self::CLICK_CONF_FILTER] = null;
        $this->values[self::CLICK_RTSE_HOLD] = null;
        $this->values[self::CSTRA] = null;
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
     * Sets value of 'gsample_log' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_gsample_log($value)
    {
        return $this->set(self::GSAMPLE_LOG, $value);
    }

    /**
     * Returns value of 'gsample_log' property
     *
     * @return string
     */
    public function get_gsample_log()
    {
        return $this->get(self::GSAMPLE_LOG);
    }

    /**
     * Sets value of 'click_weight' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_weight($value)
    {
        return $this->set(self::CLICK_WEIGHT, $value);
    }

    /**
     * Returns value of 'click_weight' property
     *
     * @return int
     */
    public function get_click_weight()
    {
        return $this->get(self::CLICK_WEIGHT);
    }

    /**
     * Sets value of 'click_weight_merge' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_weight_merge($value)
    {
        return $this->set(self::CLICK_WEIGHT_MERGE, $value);
    }

    /**
     * Returns value of 'click_weight_merge' property
     *
     * @return int
     */
    public function get_click_weight_merge()
    {
        return $this->get(self::CLICK_WEIGHT_MERGE);
    }

    /**
     * Sets value of 'click_weight_merged_time' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_weight_merged_time($value)
    {
        return $this->set(self::CLICK_WEIGHT_MERGED_TIME, $value);
    }

    /**
     * Returns value of 'click_weight_merged_time' property
     *
     * @return int
     */
    public function get_click_weight_merged_time()
    {
        return $this->get(self::CLICK_WEIGHT_MERGED_TIME);
    }

    /**
     * Sets value of 'click_weight_orig' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_weight_orig($value)
    {
        return $this->set(self::CLICK_WEIGHT_ORIG, $value);
    }

    /**
     * Returns value of 'click_weight_orig' property
     *
     * @return int
     */
    public function get_click_weight_orig()
    {
        return $this->get(self::CLICK_WEIGHT_ORIG);
    }

    /**
     * Sets value of 'click_time_weight' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_time_weight($value)
    {
        return $this->set(self::CLICK_TIME_WEIGHT, $value);
    }

    /**
     * Returns value of 'click_time_weight' property
     *
     * @return int
     */
    public function get_click_time_weight()
    {
        return $this->get(self::CLICK_TIME_WEIGHT);
    }

    /**
     * Sets value of 'click_time_ratio' property
     *
     * @param float $value Property value
     *
     * @return null
     */
    public function set_click_time_ratio($value)
    {
        return $this->set(self::CLICK_TIME_RATIO, $value);
    }

    /**
     * Returns value of 'click_time_ratio' property
     *
     * @return float
     */
    public function get_click_time_ratio()
    {
        return $this->get(self::CLICK_TIME_RATIO);
    }

    /**
     * Sets value of 'click_time_ratio_a' property
     *
     * @param float $value Property value
     *
     * @return null
     */
    public function set_click_time_ratio_a($value)
    {
        return $this->set(self::CLICK_TIME_RATIO_A, $value);
    }

    /**
     * Returns value of 'click_time_ratio_a' property
     *
     * @return float
     */
    public function get_click_time_ratio_a()
    {
        return $this->get(self::CLICK_TIME_RATIO_A);
    }

    /**
     * Sets value of 'click_time_ratio_b' property
     *
     * @param float $value Property value
     *
     * @return null
     */
    public function set_click_time_ratio_b($value)
    {
        return $this->set(self::CLICK_TIME_RATIO_B, $value);
    }

    /**
     * Returns value of 'click_time_ratio_b' property
     *
     * @return float
     */
    public function get_click_time_ratio_b()
    {
        return $this->get(self::CLICK_TIME_RATIO_B);
    }

    /**
     * Sets value of 'click_time_level' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_time_level($value)
    {
        return $this->set(self::CLICK_TIME_LEVEL, $value);
    }

    /**
     * Returns value of 'click_time_level' property
     *
     * @return int
     */
    public function get_click_time_level()
    {
        return $this->get(self::CLICK_TIME_LEVEL);
    }

    /**
     * Sets value of 'click_orig_ala_pos' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_orig_ala_pos($value)
    {
        return $this->set(self::CLICK_ORIG_ALA_POS, $value);
    }

    /**
     * Returns value of 'click_orig_ala_pos' property
     *
     * @return int
     */
    public function get_click_orig_ala_pos()
    {
        return $this->get(self::CLICK_ORIG_ALA_POS);
    }

    /**
     * Sets value of 'click_orig_pos' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_orig_pos($value)
    {
        return $this->set(self::CLICK_ORIG_POS, $value);
    }

    /**
     * Returns value of 'click_orig_pos' property
     *
     * @return int
     */
    public function get_click_orig_pos()
    {
        return $this->get(self::CLICK_ORIG_POS);
    }

    /**
     * Sets value of 'url_display' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_display($value)
    {
        return $this->set(self::URL_DISPLAY, $value);
    }

    /**
     * Returns value of 'url_display' property
     *
     * @return int
     */
    public function get_url_display()
    {
        return $this->get(self::URL_DISPLAY);
    }

    /**
     * Sets value of 'url_exam' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_exam($value)
    {
        return $this->set(self::URL_EXAM, $value);
    }

    /**
     * Returns value of 'url_exam' property
     *
     * @return int
     */
    public function get_url_exam()
    {
        return $this->get(self::URL_EXAM);
    }

    /**
     * Sets value of 'url_click' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_click($value)
    {
        return $this->set(self::URL_CLICK, $value);
    }

    /**
     * Returns value of 'url_click' property
     *
     * @return int
     */
    public function get_url_click()
    {
        return $this->get(self::URL_CLICK);
    }

    /**
     * Sets value of 'url_satisfy' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_satisfy($value)
    {
        return $this->set(self::URL_SATISFY, $value);
    }

    /**
     * Returns value of 'url_satisfy' property
     *
     * @return int
     */
    public function get_url_satisfy()
    {
        return $this->get(self::URL_SATISFY);
    }

    /**
     * Sets value of 'url_skip' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_skip($value)
    {
        return $this->set(self::URL_SKIP, $value);
    }

    /**
     * Returns value of 'url_skip' property
     *
     * @return int
     */
    public function get_url_skip()
    {
        return $this->get(self::URL_SKIP);
    }

    /**
     * Sets value of 'url_satisfy_degree' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_satisfy_degree($value)
    {
        return $this->set(self::URL_SATISFY_DEGREE, $value);
    }

    /**
     * Returns value of 'url_satisfy_degree' property
     *
     * @return int
     */
    public function get_url_satisfy_degree()
    {
        return $this->get(self::URL_SATISFY_DEGREE);
    }

    /**
     * Sets value of 'url_click_need' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_click_need($value)
    {
        return $this->set(self::URL_CLICK_NEED, $value);
    }

    /**
     * Returns value of 'url_click_need' property
     *
     * @return int
     */
    public function get_url_click_need()
    {
        return $this->get(self::URL_CLICK_NEED);
    }

    /**
     * Sets value of 'url_click_ratio' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_click_ratio($value)
    {
        return $this->set(self::URL_CLICK_RATIO, $value);
    }

    /**
     * Returns value of 'url_click_ratio' property
     *
     * @return int
     */
    public function get_url_click_ratio()
    {
        return $this->get(self::URL_CLICK_RATIO);
    }

    /**
     * Sets value of 'url_satisfy_ratio' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_satisfy_ratio($value)
    {
        return $this->set(self::URL_SATISFY_RATIO, $value);
    }

    /**
     * Returns value of 'url_satisfy_ratio' property
     *
     * @return int
     */
    public function get_url_satisfy_ratio()
    {
        return $this->get(self::URL_SATISFY_RATIO);
    }

    /**
     * Sets value of 'click_auto_hold_orig' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_click_auto_hold_orig($value)
    {
        return $this->set(self::CLICK_AUTO_HOLD_ORIG, $value);
    }

    /**
     * Returns value of 'click_auto_hold_orig' property
     *
     * @return bool
     */
    public function get_click_auto_hold_orig()
    {
        return $this->get(self::CLICK_AUTO_HOLD_ORIG);
    }

    /**
     * Sets value of 'click_conf_filter' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_click_conf_filter($value)
    {
        return $this->set(self::CLICK_CONF_FILTER, $value);
    }

    /**
     * Returns value of 'click_conf_filter' property
     *
     * @return bool
     */
    public function get_click_conf_filter()
    {
        return $this->get(self::CLICK_CONF_FILTER);
    }

    /**
     * Sets value of 'click_rtse_hold' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_click_rtse_hold($value)
    {
        return $this->set(self::CLICK_RTSE_HOLD, $value);
    }

    /**
     * Returns value of 'click_rtse_hold' property
     *
     * @return bool
     */
    public function get_click_rtse_hold()
    {
        return $this->get(self::CLICK_RTSE_HOLD);
    }

    /**
     * Sets value of 'cstra' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_cstra($value)
    {
        return $this->set(self::CSTRA, $value);
    }

    /**
     * Returns value of 'cstra' property
     *
     * @return int
     */
    public function get_cstra()
    {
        return $this->get(self::CSTRA);
    }
}

/**
 * OutInfo message
 */
class OutInfo extends ProtobufMessage
{
    /* Field index constants */
    const DEBUGINFO = 1;
    const QUERY_SEARCH = 2;
    const RESULTINFO = 3;

    /* @var string class type */
    protected static $class_type = "OutInfo";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::DEBUGINFO => array(
            'name' => 'debugInfo',
            'required' => false,
            'type' => 7,
        ),
        self::QUERY_SEARCH => array(
            'name' => 'query_search',
            'required' => false,
            'type' => 5,
        ),
        self::RESULTINFO => array(
            'name' => 'resultInfo',
            'repeated' => true,
            'type' => 'OutNodeInfo'
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
        $this->values[self::DEBUGINFO] = null;
        $this->values[self::QUERY_SEARCH] = null;
        $this->values[self::RESULTINFO] = array();
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
     * Sets value of 'debugInfo' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_debugInfo($value)
    {
        return $this->set(self::DEBUGINFO, $value);
    }

    /**
     * Returns value of 'debugInfo' property
     *
     * @return string
     */
    public function get_debugInfo()
    {
        return $this->get(self::DEBUGINFO);
    }

    /**
     * Sets value of 'query_search' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_query_search($value)
    {
        return $this->set(self::QUERY_SEARCH, $value);
    }

    /**
     * Returns value of 'query_search' property
     *
     * @return int
     */
    public function get_query_search()
    {
        return $this->get(self::QUERY_SEARCH);
    }

    /**
     * Sets value of 'resultInfo' property
     *
     * @param OutNodeInfo $value Property value
     *
     * @return null
     */
    public function set_resultInfo($value)
    {
        return $this->set(self::RESULTINFO, $value);
    }

    /**
     * Appends value to 'resultInfo' list
     *
     * @param OutNodeInfo $value Value to append
     *
     * @return null
     */
    public function append_resultInfo(OutNodeInfo $value)
    {
        return $this->append(self::RESULTINFO, $value);
    }

    /**
     * Clears 'resultInfo' list
     *
     * @return null
     */
    public function clear_resultInfo()
    {
        return $this->clear(self::RESULTINFO);
    }

    /**
     * Returns 'resultInfo' list
     *
     * @return OutNodeInfo[]
     */
    public function get_resultInfo()
    {
        return $this->get(self::RESULTINFO);
    }

    /**
     * Returns 'resultInfo' iterator
     *
     * @return ArrayIterator
     */
    public function get_resultInfoIterator()
    {
        return new ArrayIterator($this->get(self::RESULTINFO));
    }

    /**
     * Returns element from 'resultInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return OutNodeInfo
     */
    public function get_resultInfoAt($offset)
    {
        return $this->get(self::RESULTINFO, $offset);
    }

    /**
     * Returns element from 'resultInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return OutNodeInfo
     */
    public function get_resultInfoObjectAt($offset)
    {
        $value = $this->get(self::RESULTINFO, $offset);
        $instance = new OutNodeInfo();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'resultInfo' list
     *
     * @return int
     */
    public function get_resultInfoCount()
    {
        return $this->count(self::RESULTINFO);
    }
}

/**
 * query_url_features_t message
 */
class query_url_features_t extends ProtobufMessage
{
    /* Field index constants */
    const CLICK_URL_SIGN = 1;
    const FEATURES = 2;

    /* @var string class type */
    protected static $class_type = "query_url_features_t";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::CLICK_URL_SIGN => array(
            'name' => 'click_url_sign',
            'required' => false,
            'type' => 7,
        ),
        self::FEATURES => array(
            'name' => 'features',
            'repeated' => true,
            'type' => 5,
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
        $this->values[self::CLICK_URL_SIGN] = null;
        $this->values[self::FEATURES] = array();
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
     * Sets value of 'click_url_sign' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_click_url_sign($value)
    {
        return $this->set(self::CLICK_URL_SIGN, $value);
    }

    /**
     * Returns value of 'click_url_sign' property
     *
     * @return string
     */
    public function get_click_url_sign()
    {
        return $this->get(self::CLICK_URL_SIGN);
    }

    /**
     * Sets value of 'features' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_features($value)
    {
        return $this->set(self::FEATURES, $value);
    }

    /**
     * Appends value to 'features' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_features($value)
    {
        return $this->append(self::FEATURES, $value);
    }

    /**
     * Clears 'features' list
     *
     * @return null
     */
    public function clear_features()
    {
        return $this->clear(self::FEATURES);
    }

    /**
     * Returns 'features' list
     *
     * @return int[]
     */
    public function get_features()
    {
        return $this->get(self::FEATURES);
    }

    /**
     * Returns 'features' iterator
     *
     * @return ArrayIterator
     */
    public function get_featuresIterator()
    {
        return new ArrayIterator($this->get(self::FEATURES));
    }

    /**
     * Returns element from 'features' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_featuresAt($offset)
    {
        return $this->get(self::FEATURES, $offset);
    }

    /**
     * Returns count of 'features' list
     *
     * @return int
     */
    public function get_featuresCount()
    {
        return $this->count(self::FEATURES);
    }
}

/**
 * click_features_t message
 */
class click_features_t extends ProtobufMessage
{
    /* Field index constants */
    const QUERY_SEARCH = 1;
    const QUERY_CLICK = 2;
    const QUERY_SATISFY = 3;
    const TIME_QUERY_SEARCH = 4;
    const QUERY_FEATURE = 5;
    const QUERY_URL_FEAS = 6;
    const MORE_QUERY_URL_FEAS = 7;
    const TIME_QUERY_URL_FEAS = 8;

    /* @var string class type */
    protected static $class_type = "click_features_t";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::QUERY_SEARCH => array(
            'name' => 'query_search',
            'required' => false,
            'type' => 5,
        ),
        self::QUERY_CLICK => array(
            'name' => 'query_click',
            'required' => false,
            'type' => 5,
        ),
        self::QUERY_SATISFY => array(
            'name' => 'query_satisfy',
            'required' => false,
            'type' => 5,
        ),
        self::TIME_QUERY_SEARCH => array(
            'name' => 'time_query_search',
            'required' => false,
            'type' => 5,
        ),
        self::QUERY_FEATURE => array(
            'name' => 'query_feature',
            'repeated' => true,
            'type' => 5,
        ),
        self::QUERY_URL_FEAS => array(
            'name' => 'query_url_feas',
            'repeated' => true,
            'type' => 'query_url_features_t'
        ),
        self::MORE_QUERY_URL_FEAS => array(
            'name' => 'more_query_url_feas',
            'repeated' => true,
            'type' => 'query_url_features_t'
        ),
        self::TIME_QUERY_URL_FEAS => array(
            'name' => 'time_query_url_feas',
            'repeated' => true,
            'type' => 'query_url_features_t'
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
        $this->values[self::QUERY_SEARCH] = null;
        $this->values[self::QUERY_CLICK] = null;
        $this->values[self::QUERY_SATISFY] = null;
        $this->values[self::TIME_QUERY_SEARCH] = null;
        $this->values[self::QUERY_FEATURE] = array();
        $this->values[self::QUERY_URL_FEAS] = array();
        $this->values[self::MORE_QUERY_URL_FEAS] = array();
        $this->values[self::TIME_QUERY_URL_FEAS] = array();
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
     * Sets value of 'query_search' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_query_search($value)
    {
        return $this->set(self::QUERY_SEARCH, $value);
    }

    /**
     * Returns value of 'query_search' property
     *
     * @return int
     */
    public function get_query_search()
    {
        return $this->get(self::QUERY_SEARCH);
    }

    /**
     * Sets value of 'query_click' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_query_click($value)
    {
        return $this->set(self::QUERY_CLICK, $value);
    }

    /**
     * Returns value of 'query_click' property
     *
     * @return int
     */
    public function get_query_click()
    {
        return $this->get(self::QUERY_CLICK);
    }

    /**
     * Sets value of 'query_satisfy' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_query_satisfy($value)
    {
        return $this->set(self::QUERY_SATISFY, $value);
    }

    /**
     * Returns value of 'query_satisfy' property
     *
     * @return int
     */
    public function get_query_satisfy()
    {
        return $this->get(self::QUERY_SATISFY);
    }

    /**
     * Sets value of 'time_query_search' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_time_query_search($value)
    {
        return $this->set(self::TIME_QUERY_SEARCH, $value);
    }

    /**
     * Returns value of 'time_query_search' property
     *
     * @return int
     */
    public function get_time_query_search()
    {
        return $this->get(self::TIME_QUERY_SEARCH);
    }

    /**
     * Sets value of 'query_feature' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_query_feature($value)
    {
        return $this->set(self::QUERY_FEATURE, $value);
    }

    /**
     * Appends value to 'query_feature' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_query_feature($value)
    {
        return $this->append(self::QUERY_FEATURE, $value);
    }

    /**
     * Clears 'query_feature' list
     *
     * @return null
     */
    public function clear_query_feature()
    {
        return $this->clear(self::QUERY_FEATURE);
    }

    /**
     * Returns 'query_feature' list
     *
     * @return int[]
     */
    public function get_query_feature()
    {
        return $this->get(self::QUERY_FEATURE);
    }

    /**
     * Returns 'query_feature' iterator
     *
     * @return ArrayIterator
     */
    public function get_query_featureIterator()
    {
        return new ArrayIterator($this->get(self::QUERY_FEATURE));
    }

    /**
     * Returns element from 'query_feature' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_query_featureAt($offset)
    {
        return $this->get(self::QUERY_FEATURE, $offset);
    }

    /**
     * Returns count of 'query_feature' list
     *
     * @return int
     */
    public function get_query_featureCount()
    {
        return $this->count(self::QUERY_FEATURE);
    }

    /**
     * Sets value of 'query_url_feas' property
     *
     * @param query_url_features_t $value Property value
     *
     * @return null
     */
    public function set_query_url_feas($value)
    {
        return $this->set(self::QUERY_URL_FEAS, $value);
    }

    /**
     * Appends value to 'query_url_feas' list
     *
     * @param query_url_features_t $value Value to append
     *
     * @return null
     */
    public function append_query_url_feas(query_url_features_t $value)
    {
        return $this->append(self::QUERY_URL_FEAS, $value);
    }

    /**
     * Clears 'query_url_feas' list
     *
     * @return null
     */
    public function clear_query_url_feas()
    {
        return $this->clear(self::QUERY_URL_FEAS);
    }

    /**
     * Returns 'query_url_feas' list
     *
     * @return query_url_features_t[]
     */
    public function get_query_url_feas()
    {
        return $this->get(self::QUERY_URL_FEAS);
    }

    /**
     * Returns 'query_url_feas' iterator
     *
     * @return ArrayIterator
     */
    public function get_query_url_feasIterator()
    {
        return new ArrayIterator($this->get(self::QUERY_URL_FEAS));
    }

    /**
     * Returns element from 'query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_query_url_feasAt($offset)
    {
        return $this->get(self::QUERY_URL_FEAS, $offset);
    }

    /**
     * Returns element from 'query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_query_url_feasObjectAt($offset)
    {
        $value = $this->get(self::QUERY_URL_FEAS, $offset);
        $instance = new query_url_features_t();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'query_url_feas' list
     *
     * @return int
     */
    public function get_query_url_feasCount()
    {
        return $this->count(self::QUERY_URL_FEAS);
    }

    /**
     * Sets value of 'more_query_url_feas' property
     *
     * @param query_url_features_t $value Property value
     *
     * @return null
     */
    public function set_more_query_url_feas($value)
    {
        return $this->set(self::MORE_QUERY_URL_FEAS, $value);
    }

    /**
     * Appends value to 'more_query_url_feas' list
     *
     * @param query_url_features_t $value Value to append
     *
     * @return null
     */
    public function append_more_query_url_feas(query_url_features_t $value)
    {
        return $this->append(self::MORE_QUERY_URL_FEAS, $value);
    }

    /**
     * Clears 'more_query_url_feas' list
     *
     * @return null
     */
    public function clear_more_query_url_feas()
    {
        return $this->clear(self::MORE_QUERY_URL_FEAS);
    }

    /**
     * Returns 'more_query_url_feas' list
     *
     * @return query_url_features_t[]
     */
    public function get_more_query_url_feas()
    {
        return $this->get(self::MORE_QUERY_URL_FEAS);
    }

    /**
     * Returns 'more_query_url_feas' iterator
     *
     * @return ArrayIterator
     */
    public function get_more_query_url_feasIterator()
    {
        return new ArrayIterator($this->get(self::MORE_QUERY_URL_FEAS));
    }

    /**
     * Returns element from 'more_query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_more_query_url_feasAt($offset)
    {
        return $this->get(self::MORE_QUERY_URL_FEAS, $offset);
    }

    /**
     * Returns element from 'more_query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_more_query_url_feasObjectAt($offset)
    {
        $value = $this->get(self::MORE_QUERY_URL_FEAS, $offset);
        $instance = new query_url_features_t();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'more_query_url_feas' list
     *
     * @return int
     */
    public function get_more_query_url_feasCount()
    {
        return $this->count(self::MORE_QUERY_URL_FEAS);
    }

    /**
     * Sets value of 'time_query_url_feas' property
     *
     * @param query_url_features_t $value Property value
     *
     * @return null
     */
    public function set_time_query_url_feas($value)
    {
        return $this->set(self::TIME_QUERY_URL_FEAS, $value);
    }

    /**
     * Appends value to 'time_query_url_feas' list
     *
     * @param query_url_features_t $value Value to append
     *
     * @return null
     */
    public function append_time_query_url_feas(query_url_features_t $value)
    {
        return $this->append(self::TIME_QUERY_URL_FEAS, $value);
    }

    /**
     * Clears 'time_query_url_feas' list
     *
     * @return null
     */
    public function clear_time_query_url_feas()
    {
        return $this->clear(self::TIME_QUERY_URL_FEAS);
    }

    /**
     * Returns 'time_query_url_feas' list
     *
     * @return query_url_features_t[]
     */
    public function get_time_query_url_feas()
    {
        return $this->get(self::TIME_QUERY_URL_FEAS);
    }

    /**
     * Returns 'time_query_url_feas' iterator
     *
     * @return ArrayIterator
     */
    public function get_time_query_url_feasIterator()
    {
        return new ArrayIterator($this->get(self::TIME_QUERY_URL_FEAS));
    }

    /**
     * Returns element from 'time_query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_time_query_url_feasAt($offset)
    {
        return $this->get(self::TIME_QUERY_URL_FEAS, $offset);
    }

    /**
     * Returns element from 'time_query_url_feas' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return query_url_features_t
     */
    public function get_time_query_url_feasObjectAt($offset)
    {
        $value = $this->get(self::TIME_QUERY_URL_FEAS, $offset);
        $instance = new query_url_features_t();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'time_query_url_feas' list
     *
     * @return int
     */
    public function get_time_query_url_feasCount()
    {
        return $this->count(self::TIME_QUERY_URL_FEAS);
    }
}

/**
 * QueryInfo message
 */
class QueryInfo extends ProtobufMessage
{
    /* Field index constants */
    const QUERY_WORD = 1;
    const QUERYID = 2;
    const RESULTLANG = 3;
    const PRE_QUERY = 4;
    const URI_MD5 = 5;
    const URL_CANNOT_CLUSTER_SRCID_LIST = 6;
    const SITE_CANNOT_CLUSTER_SRCID_LIST = 7;
    const SIDS = 8;
    const SRCID_LIST = 9;
    const DEBUGINFO = 10;
    const CLICK_FEATURES = 11;

    /* @var string class type */
    protected static $class_type = "QueryInfo";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::QUERY_WORD => array(
            'name' => 'query_word',
            'required' => false,
            'type' => 7,
        ),
        self::QUERYID => array(
            'name' => 'queryId',
            'required' => false,
            'type' => 7,
        ),
        self::RESULTLANG => array(
            'name' => 'resultLang',
            'required' => false,
            'type' => 7,
        ),
        self::PRE_QUERY => array(
            'name' => 'pre_query',
            'required' => false,
            'type' => 7,
        ),
        self::URI_MD5 => array(
            'name' => 'uri_md5',
            'required' => false,
            'type' => 7,
        ),
        self::URL_CANNOT_CLUSTER_SRCID_LIST => array(
            'name' => 'url_cannot_cluster_srcid_list',
            'repeated' => true,
            'type' => 5,
        ),
        self::SITE_CANNOT_CLUSTER_SRCID_LIST => array(
            'name' => 'site_cannot_cluster_srcid_list',
            'repeated' => true,
            'type' => 5,
        ),
        self::SIDS => array(
            'name' => 'sids',
            'repeated' => true,
            'type' => 5,
        ),
        self::SRCID_LIST => array(
            'name' => 'srcid_list',
            'repeated' => true,
            'type' => 5,
        ),
        self::DEBUGINFO => array(
            'name' => 'debugInfo',
            'required' => false,
            'type' => 7,
        ),
        self::CLICK_FEATURES => array(
            'name' => 'click_features',
            'required' => false,
            'type' => 'click_features_t'
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
        $this->values[self::QUERY_WORD] = null;
        $this->values[self::QUERYID] = null;
        $this->values[self::RESULTLANG] = null;
        $this->values[self::PRE_QUERY] = null;
        $this->values[self::URI_MD5] = null;
        $this->values[self::URL_CANNOT_CLUSTER_SRCID_LIST] = array();
        $this->values[self::SITE_CANNOT_CLUSTER_SRCID_LIST] = array();
        $this->values[self::SIDS] = array();
        $this->values[self::SRCID_LIST] = array();
        $this->values[self::DEBUGINFO] = null;
        $this->values[self::CLICK_FEATURES] = null;
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
     * Sets value of 'query_word' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_query_word($value)
    {
        return $this->set(self::QUERY_WORD, $value);
    }

    /**
     * Returns value of 'query_word' property
     *
     * @return string
     */
    public function get_query_word()
    {
        return $this->get(self::QUERY_WORD);
    }

    /**
     * Sets value of 'queryId' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_queryId($value)
    {
        return $this->set(self::QUERYID, $value);
    }

    /**
     * Returns value of 'queryId' property
     *
     * @return string
     */
    public function get_queryId()
    {
        return $this->get(self::QUERYID);
    }

    /**
     * Sets value of 'resultLang' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_resultLang($value)
    {
        return $this->set(self::RESULTLANG, $value);
    }

    /**
     * Returns value of 'resultLang' property
     *
     * @return string
     */
    public function get_resultLang()
    {
        return $this->get(self::RESULTLANG);
    }

    /**
     * Sets value of 'pre_query' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_pre_query($value)
    {
        return $this->set(self::PRE_QUERY, $value);
    }

    /**
     * Returns value of 'pre_query' property
     *
     * @return string
     */
    public function get_pre_query()
    {
        return $this->get(self::PRE_QUERY);
    }

    /**
     * Sets value of 'uri_md5' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_uri_md5($value)
    {
        return $this->set(self::URI_MD5, $value);
    }

    /**
     * Returns value of 'uri_md5' property
     *
     * @return string
     */
    public function get_uri_md5()
    {
        return $this->get(self::URI_MD5);
    }

    /**
     * Sets value of 'url_cannot_cluster_srcid_list' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_cannot_cluster_srcid_list($value)
    {
        return $this->set(self::URL_CANNOT_CLUSTER_SRCID_LIST, $value);
    }

    /**
     * Appends value to 'url_cannot_cluster_srcid_list' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_url_cannot_cluster_srcid_list($value)
    {
        return $this->append(self::URL_CANNOT_CLUSTER_SRCID_LIST, $value);
    }

    /**
     * Clears 'url_cannot_cluster_srcid_list' list
     *
     * @return null
     */
    public function clear_url_cannot_cluster_srcid_list()
    {
        return $this->clear(self::URL_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Returns 'url_cannot_cluster_srcid_list' list
     *
     * @return int[]
     */
    public function get_url_cannot_cluster_srcid_list()
    {
        return $this->get(self::URL_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Returns 'url_cannot_cluster_srcid_list' iterator
     *
     * @return ArrayIterator
     */
    public function get_url_cannot_cluster_srcid_listIterator()
    {
        return new ArrayIterator($this->get(self::URL_CANNOT_CLUSTER_SRCID_LIST));
    }

    /**
     * Returns element from 'url_cannot_cluster_srcid_list' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_url_cannot_cluster_srcid_listAt($offset)
    {
        return $this->get(self::URL_CANNOT_CLUSTER_SRCID_LIST, $offset);
    }

    /**
     * Returns count of 'url_cannot_cluster_srcid_list' list
     *
     * @return int
     */
    public function get_url_cannot_cluster_srcid_listCount()
    {
        return $this->count(self::URL_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Sets value of 'site_cannot_cluster_srcid_list' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_site_cannot_cluster_srcid_list($value)
    {
        return $this->set(self::SITE_CANNOT_CLUSTER_SRCID_LIST, $value);
    }

    /**
     * Appends value to 'site_cannot_cluster_srcid_list' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_site_cannot_cluster_srcid_list($value)
    {
        return $this->append(self::SITE_CANNOT_CLUSTER_SRCID_LIST, $value);
    }

    /**
     * Clears 'site_cannot_cluster_srcid_list' list
     *
     * @return null
     */
    public function clear_site_cannot_cluster_srcid_list()
    {
        return $this->clear(self::SITE_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Returns 'site_cannot_cluster_srcid_list' list
     *
     * @return int[]
     */
    public function get_site_cannot_cluster_srcid_list()
    {
        return $this->get(self::SITE_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Returns 'site_cannot_cluster_srcid_list' iterator
     *
     * @return ArrayIterator
     */
    public function get_site_cannot_cluster_srcid_listIterator()
    {
        return new ArrayIterator($this->get(self::SITE_CANNOT_CLUSTER_SRCID_LIST));
    }

    /**
     * Returns element from 'site_cannot_cluster_srcid_list' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_site_cannot_cluster_srcid_listAt($offset)
    {
        return $this->get(self::SITE_CANNOT_CLUSTER_SRCID_LIST, $offset);
    }

    /**
     * Returns count of 'site_cannot_cluster_srcid_list' list
     *
     * @return int
     */
    public function get_site_cannot_cluster_srcid_listCount()
    {
        return $this->count(self::SITE_CANNOT_CLUSTER_SRCID_LIST);
    }

    /**
     * Sets value of 'sids' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_sids($value)
    {
        return $this->set(self::SIDS, $value);
    }

    /**
     * Appends value to 'sids' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_sids($value)
    {
        return $this->append(self::SIDS, $value);
    }

    /**
     * Clears 'sids' list
     *
     * @return null
     */
    public function clear_sids()
    {
        return $this->clear(self::SIDS);
    }

    /**
     * Returns 'sids' list
     *
     * @return int[]
     */
    public function get_sids()
    {
        return $this->get(self::SIDS);
    }

    /**
     * Returns 'sids' iterator
     *
     * @return ArrayIterator
     */
    public function get_sidsIterator()
    {
        return new ArrayIterator($this->get(self::SIDS));
    }

    /**
     * Returns element from 'sids' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_sidsAt($offset)
    {
        return $this->get(self::SIDS, $offset);
    }

    /**
     * Returns count of 'sids' list
     *
     * @return int
     */
    public function get_sidsCount()
    {
        return $this->count(self::SIDS);
    }

    /**
     * Sets value of 'srcid_list' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_srcid_list($value)
    {
        return $this->set(self::SRCID_LIST, $value);
    }

    /**
     * Appends value to 'srcid_list' list
     *
     * @param int $value Value to append
     *
     * @return null
     */
    public function append_srcid_list($value)
    {
        return $this->append(self::SRCID_LIST, $value);
    }

    /**
     * Clears 'srcid_list' list
     *
     * @return null
     */
    public function clear_srcid_list()
    {
        return $this->clear(self::SRCID_LIST);
    }

    /**
     * Returns 'srcid_list' list
     *
     * @return int[]
     */
    public function get_srcid_list()
    {
        return $this->get(self::SRCID_LIST);
    }

    /**
     * Returns 'srcid_list' iterator
     *
     * @return ArrayIterator
     */
    public function get_srcid_listIterator()
    {
        return new ArrayIterator($this->get(self::SRCID_LIST));
    }

    /**
     * Returns element from 'srcid_list' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return int
     */
    public function get_srcid_listAt($offset)
    {
        return $this->get(self::SRCID_LIST, $offset);
    }

    /**
     * Returns count of 'srcid_list' list
     *
     * @return int
     */
    public function get_srcid_listCount()
    {
        return $this->count(self::SRCID_LIST);
    }

    /**
     * Sets value of 'debugInfo' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_debugInfo($value)
    {
        return $this->set(self::DEBUGINFO, $value);
    }

    /**
     * Returns value of 'debugInfo' property
     *
     * @return string
     */
    public function get_debugInfo()
    {
        return $this->get(self::DEBUGINFO);
    }

    /**
     * Sets value of 'click_features' property
     *
     * @param click_features_t $value Property value
     *
     * @return null
     */
    public function set_click_features(click_features_t $value)
    {
        return $this->set(self::CLICK_FEATURES, $value);
    }

    /**
     * Returns value of 'click_features' property
     *
     * @return click_features_t
     */
    public function get_click_features()
    {
        return $this->get(self::CLICK_FEATURES);
    }

    /**
     * Returns value of 'click_features' property
     *
     * @return click_features_t
     */
    public function get_click_featuresObject()
    {
        $value = $this->get(self::CLICK_FEATURES);
        $instance = new click_features_t();
        $instance->copy($value);
        return $instance;
    }
}

/**
 * offset_t message
 */
class offset_t extends ProtobufMessage
{
    /* Field index constants */
    const TITLE = 1;
    const URL = 2;

    /* @var string class type */
    protected static $class_type = "offset_t";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::TITLE => array(
            'name' => 'title',
            'required' => false,
            'type' => 7,
        ),
        self::URL => array(
            'name' => 'url',
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
        $this->values[self::TITLE] = null;
        $this->values[self::URL] = null;
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
     * Sets value of 'title' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_title($value)
    {
        return $this->set(self::TITLE, $value);
    }

    /**
     * Returns value of 'title' property
     *
     * @return string
     */
    public function get_title()
    {
        return $this->get(self::TITLE);
    }

    /**
     * Sets value of 'url' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_url($value)
    {
        return $this->set(self::URL, $value);
    }

    /**
     * Returns value of 'url' property
     *
     * @return string
     */
    public function get_url()
    {
        return $this->get(self::URL);
    }
}

/**
 * SortInfo message
 */
class SortInfo extends ProtobufMessage
{
    /* Field index constants */
    const SRCID = 1;
    const WEIGHT = 2;
    const SORT = 3;

    /* @var string class type */
    protected static $class_type = "SortInfo";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::SRCID => array(
            'name' => 'srcid',
            'required' => false,
            'type' => 5,
        ),
        self::WEIGHT => array(
            'name' => 'weight',
            'required' => false,
            'type' => 5,
        ),
        self::SORT => array(
            'name' => 'sort',
            'required' => false,
            'type' => 5,
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
        $this->values[self::SRCID] = null;
        $this->values[self::WEIGHT] = null;
        $this->values[self::SORT] = null;
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
     * Sets value of 'srcid' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_srcid($value)
    {
        return $this->set(self::SRCID, $value);
    }

    /**
     * Returns value of 'srcid' property
     *
     * @return int
     */
    public function get_srcid()
    {
        return $this->get(self::SRCID);
    }

    /**
     * Sets value of 'weight' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_weight($value)
    {
        return $this->set(self::WEIGHT, $value);
    }

    /**
     * Returns value of 'weight' property
     *
     * @return int
     */
    public function get_weight()
    {
        return $this->get(self::WEIGHT);
    }

    /**
     * Sets value of 'sort' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_sort($value)
    {
        return $this->set(self::SORT, $value);
    }

    /**
     * Returns value of 'sort' property
     *
     * @return int
     */
    public function get_sort()
    {
        return $this->get(self::SORT);
    }
}

/**
 * SortFactor message
 */
class SortFactor extends ProtobufMessage
{
    /* Field index constants */
    const ZHIDA = 1;
    const EXTALADDIN = 2;
    const EXTSYNALADDIN = 3;

    /* @var string class type */
    protected static $class_type = "SortFactor";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::ZHIDA => array(
            'name' => 'zhida',
            'required' => false,
            'type' => 8,
        ),
        self::EXTALADDIN => array(
            'name' => 'extAladdin',
            'required' => false,
            'type' => 8,
        ),
        self::EXTSYNALADDIN => array(
            'name' => 'extSynAladdin',
            'required' => false,
            'type' => 8,
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
        $this->values[self::ZHIDA] = null;
        $this->values[self::EXTALADDIN] = null;
        $this->values[self::EXTSYNALADDIN] = null;
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
     * Sets value of 'zhida' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_zhida($value)
    {
        return $this->set(self::ZHIDA, $value);
    }

    /**
     * Returns value of 'zhida' property
     *
     * @return bool
     */
    public function get_zhida()
    {
        return $this->get(self::ZHIDA);
    }

    /**
     * Sets value of 'extAladdin' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_extAladdin($value)
    {
        return $this->set(self::EXTALADDIN, $value);
    }

    /**
     * Returns value of 'extAladdin' property
     *
     * @return bool
     */
    public function get_extAladdin()
    {
        return $this->get(self::EXTALADDIN);
    }

    /**
     * Sets value of 'extSynAladdin' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_extSynAladdin($value)
    {
        return $this->set(self::EXTSYNALADDIN, $value);
    }

    /**
     * Returns value of 'extSynAladdin' property
     *
     * @return bool
     */
    public function get_extSynAladdin()
    {
        return $this->get(self::EXTSYNALADDIN);
    }
}

/**
 * NodeInfo message
 */
class NodeInfo extends ProtobufMessage
{
    /* Field index constants */
    const SOURCE = 1;
    const COMEFROME = 2;
    const OFFSETINFO = 3;
    const SITESIGN = 4;
    const URLSIGNHIGH32 = 5;
    const URLSIGNLOW32 = 6;
    const UNIURLSIGNHIGH32 = 7;
    const UNIURLSIGNLOW32 = 8;
    const GSAMPLELOG = 9;
    const SORTINFO = 10;
    const CLICK_URL_SIGN = 11;
    const CLICK_TUICHANG = 12;
    const NEED_CLICK_ADJUST = 13;
    const URL_ZDZX_ON = 14;
    const URL_ZDZX_TC = 15;
    const UAP_SIGNED_KEY_FOR_QUERY_URL = 16;
    const UAP_SIGNED_KEY_FOR_SITE = 17;
    const UAP_SIGNED_KEY_FOR_URL = 18;
    const CLICK_PRED_POS = 19;
    const CARD_Q = 20;
    const SORTFACTOR = 21;
    const PERSONAL_EOFF = 22;
    const SHOWGW = 23;
    const YUNYING_MERGE_SHIA = 24;

    /* @var string class type */
    protected static $class_type = "NodeInfo";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::SOURCE => array(
            'name' => 'source',
            'required' => false,
            'type' => 5,
        ),
        self::COMEFROME => array(
            'name' => 'comeFrome',
            'required' => false,
            'type' => 7,
        ),
        self::OFFSETINFO => array(
            'name' => 'offsetInfo',
            'required' => false,
            'type' => 'offset_t'
        ),
        self::SITESIGN => array(
            'name' => 'siteSign',
            'required' => false,
            'type' => 7,
        ),
        self::URLSIGNHIGH32 => array(
            'name' => 'urlSignHigh32',
            'required' => false,
            'type' => 5,
        ),
        self::URLSIGNLOW32 => array(
            'name' => 'urlSignLow32',
            'required' => false,
            'type' => 5,
        ),
        self::UNIURLSIGNHIGH32 => array(
            'name' => 'uniUrlSignHigh32',
            'required' => false,
            'type' => 5,
        ),
        self::UNIURLSIGNLOW32 => array(
            'name' => 'uniUrlSignLow32',
            'required' => false,
            'type' => 5,
        ),
        self::GSAMPLELOG => array(
            'name' => 'gSampleLog',
            'required' => false,
            'type' => 7,
        ),
        self::SORTINFO => array(
            'name' => 'sortInfo',
            'required' => false,
            'type' => 'SortInfo'
        ),
        self::CLICK_URL_SIGN => array(
            'name' => 'click_url_sign',
            'required' => false,
            'type' => 7,
        ),
        self::CLICK_TUICHANG => array(
            'name' => 'click_tuichang',
            'required' => false,
            'type' => 5,
        ),
        self::NEED_CLICK_ADJUST => array(
            'name' => 'need_click_adjust',
            'required' => false,
            'type' => 8,
        ),
        self::URL_ZDZX_ON => array(
            'name' => 'url_zdzx_on',
            'required' => false,
            'type' => 5,
        ),
        self::URL_ZDZX_TC => array(
            'name' => 'url_zdzx_tc',
            'required' => false,
            'type' => 5,
        ),
        self::UAP_SIGNED_KEY_FOR_QUERY_URL => array(
            'name' => 'uap_signed_key_for_query_url',
            'required' => false,
            'type' => 7,
        ),
        self::UAP_SIGNED_KEY_FOR_SITE => array(
            'name' => 'uap_signed_key_for_site',
            'required' => false,
            'type' => 7,
        ),
        self::UAP_SIGNED_KEY_FOR_URL => array(
            'name' => 'uap_signed_key_for_url',
            'required' => false,
            'type' => 7,
        ),
        self::CLICK_PRED_POS => array(
            'name' => 'click_pred_pos',
            'required' => false,
            'type' => 5,
        ),
        self::CARD_Q => array(
            'name' => 'card_q',
            'required' => false,
            'type' => 1,
        ),
        self::SORTFACTOR => array(
            'name' => 'sortFactor',
            'required' => false,
            'type' => 'SortFactor'
        ),
        self::PERSONAL_EOFF => array(
            'name' => 'personal_eoff',
            'repeated' => true,
            'type' => 1,
        ),
        self::SHOWGW => array(
            'name' => 'showgw',
            'required' => false,
            'type' => 8,
        ),
        self::YUNYING_MERGE_SHIA => array(
            'name' => 'yunying_merge_shia',
            'required' => false,
            'type' => 8,
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
        $this->values[self::SOURCE] = null;
        $this->values[self::COMEFROME] = null;
        $this->values[self::OFFSETINFO] = null;
        $this->values[self::SITESIGN] = null;
        $this->values[self::URLSIGNHIGH32] = null;
        $this->values[self::URLSIGNLOW32] = null;
        $this->values[self::UNIURLSIGNHIGH32] = null;
        $this->values[self::UNIURLSIGNLOW32] = null;
        $this->values[self::GSAMPLELOG] = null;
        $this->values[self::SORTINFO] = null;
        $this->values[self::CLICK_URL_SIGN] = null;
        $this->values[self::CLICK_TUICHANG] = null;
        $this->values[self::NEED_CLICK_ADJUST] = null;
        $this->values[self::URL_ZDZX_ON] = null;
        $this->values[self::URL_ZDZX_TC] = null;
        $this->values[self::UAP_SIGNED_KEY_FOR_QUERY_URL] = null;
        $this->values[self::UAP_SIGNED_KEY_FOR_SITE] = null;
        $this->values[self::UAP_SIGNED_KEY_FOR_URL] = null;
        $this->values[self::CLICK_PRED_POS] = null;
        $this->values[self::CARD_Q] = null;
        $this->values[self::SORTFACTOR] = null;
        $this->values[self::PERSONAL_EOFF] = array();
        $this->values[self::SHOWGW] = null;
        $this->values[self::YUNYING_MERGE_SHIA] = null;
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
     * Sets value of 'source' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_source($value)
    {
        return $this->set(self::SOURCE, $value);
    }

    /**
     * Returns value of 'source' property
     *
     * @return int
     */
    public function get_source()
    {
        return $this->get(self::SOURCE);
    }

    /**
     * Sets value of 'comeFrome' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_comeFrome($value)
    {
        return $this->set(self::COMEFROME, $value);
    }

    /**
     * Returns value of 'comeFrome' property
     *
     * @return string
     */
    public function get_comeFrome()
    {
        return $this->get(self::COMEFROME);
    }

    /**
     * Sets value of 'offsetInfo' property
     *
     * @param offset_t $value Property value
     *
     * @return null
     */
    public function set_offsetInfo(offset_t $value)
    {
        return $this->set(self::OFFSETINFO, $value);
    }

    /**
     * Returns value of 'offsetInfo' property
     *
     * @return offset_t
     */
    public function get_offsetInfo()
    {
        return $this->get(self::OFFSETINFO);
    }

    /**
     * Returns value of 'offsetInfo' property
     *
     * @return offset_t
     */
    public function get_offsetInfoObject()
    {
        $value = $this->get(self::OFFSETINFO);
        $instance = new offset_t();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Sets value of 'siteSign' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_siteSign($value)
    {
        return $this->set(self::SITESIGN, $value);
    }

    /**
     * Returns value of 'siteSign' property
     *
     * @return string
     */
    public function get_siteSign()
    {
        return $this->get(self::SITESIGN);
    }

    /**
     * Sets value of 'urlSignHigh32' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_urlSignHigh32($value)
    {
        return $this->set(self::URLSIGNHIGH32, $value);
    }

    /**
     * Returns value of 'urlSignHigh32' property
     *
     * @return int
     */
    public function get_urlSignHigh32()
    {
        return $this->get(self::URLSIGNHIGH32);
    }

    /**
     * Sets value of 'urlSignLow32' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_urlSignLow32($value)
    {
        return $this->set(self::URLSIGNLOW32, $value);
    }

    /**
     * Returns value of 'urlSignLow32' property
     *
     * @return int
     */
    public function get_urlSignLow32()
    {
        return $this->get(self::URLSIGNLOW32);
    }

    /**
     * Sets value of 'uniUrlSignHigh32' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_uniUrlSignHigh32($value)
    {
        return $this->set(self::UNIURLSIGNHIGH32, $value);
    }

    /**
     * Returns value of 'uniUrlSignHigh32' property
     *
     * @return int
     */
    public function get_uniUrlSignHigh32()
    {
        return $this->get(self::UNIURLSIGNHIGH32);
    }

    /**
     * Sets value of 'uniUrlSignLow32' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_uniUrlSignLow32($value)
    {
        return $this->set(self::UNIURLSIGNLOW32, $value);
    }

    /**
     * Returns value of 'uniUrlSignLow32' property
     *
     * @return int
     */
    public function get_uniUrlSignLow32()
    {
        return $this->get(self::UNIURLSIGNLOW32);
    }

    /**
     * Sets value of 'gSampleLog' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_gSampleLog($value)
    {
        return $this->set(self::GSAMPLELOG, $value);
    }

    /**
     * Returns value of 'gSampleLog' property
     *
     * @return string
     */
    public function get_gSampleLog()
    {
        return $this->get(self::GSAMPLELOG);
    }

    /**
     * Sets value of 'sortInfo' property
     *
     * @param SortInfo $value Property value
     *
     * @return null
     */
    public function set_sortInfo(SortInfo $value)
    {
        return $this->set(self::SORTINFO, $value);
    }

    /**
     * Returns value of 'sortInfo' property
     *
     * @return SortInfo
     */
    public function get_sortInfo()
    {
        return $this->get(self::SORTINFO);
    }

    /**
     * Returns value of 'sortInfo' property
     *
     * @return SortInfo
     */
    public function get_sortInfoObject()
    {
        $value = $this->get(self::SORTINFO);
        $instance = new SortInfo();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Sets value of 'click_url_sign' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_click_url_sign($value)
    {
        return $this->set(self::CLICK_URL_SIGN, $value);
    }

    /**
     * Returns value of 'click_url_sign' property
     *
     * @return string
     */
    public function get_click_url_sign()
    {
        return $this->get(self::CLICK_URL_SIGN);
    }

    /**
     * Sets value of 'click_tuichang' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_tuichang($value)
    {
        return $this->set(self::CLICK_TUICHANG, $value);
    }

    /**
     * Returns value of 'click_tuichang' property
     *
     * @return int
     */
    public function get_click_tuichang()
    {
        return $this->get(self::CLICK_TUICHANG);
    }

    /**
     * Sets value of 'need_click_adjust' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_need_click_adjust($value)
    {
        return $this->set(self::NEED_CLICK_ADJUST, $value);
    }

    /**
     * Returns value of 'need_click_adjust' property
     *
     * @return bool
     */
    public function get_need_click_adjust()
    {
        return $this->get(self::NEED_CLICK_ADJUST);
    }

    /**
     * Sets value of 'url_zdzx_on' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_zdzx_on($value)
    {
        return $this->set(self::URL_ZDZX_ON, $value);
    }

    /**
     * Returns value of 'url_zdzx_on' property
     *
     * @return int
     */
    public function get_url_zdzx_on()
    {
        return $this->get(self::URL_ZDZX_ON);
    }

    /**
     * Sets value of 'url_zdzx_tc' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_url_zdzx_tc($value)
    {
        return $this->set(self::URL_ZDZX_TC, $value);
    }

    /**
     * Returns value of 'url_zdzx_tc' property
     *
     * @return int
     */
    public function get_url_zdzx_tc()
    {
        return $this->get(self::URL_ZDZX_TC);
    }

    /**
     * Sets value of 'uap_signed_key_for_query_url' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_uap_signed_key_for_query_url($value)
    {
        return $this->set(self::UAP_SIGNED_KEY_FOR_QUERY_URL, $value);
    }

    /**
     * Returns value of 'uap_signed_key_for_query_url' property
     *
     * @return string
     */
    public function get_uap_signed_key_for_query_url()
    {
        return $this->get(self::UAP_SIGNED_KEY_FOR_QUERY_URL);
    }

    /**
     * Sets value of 'uap_signed_key_for_site' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_uap_signed_key_for_site($value)
    {
        return $this->set(self::UAP_SIGNED_KEY_FOR_SITE, $value);
    }

    /**
     * Returns value of 'uap_signed_key_for_site' property
     *
     * @return string
     */
    public function get_uap_signed_key_for_site()
    {
        return $this->get(self::UAP_SIGNED_KEY_FOR_SITE);
    }

    /**
     * Sets value of 'uap_signed_key_for_url' property
     *
     * @param string $value Property value
     *
     * @return null
     */
    public function set_uap_signed_key_for_url($value)
    {
        return $this->set(self::UAP_SIGNED_KEY_FOR_URL, $value);
    }

    /**
     * Returns value of 'uap_signed_key_for_url' property
     *
     * @return string
     */
    public function get_uap_signed_key_for_url()
    {
        return $this->get(self::UAP_SIGNED_KEY_FOR_URL);
    }

    /**
     * Sets value of 'click_pred_pos' property
     *
     * @param int $value Property value
     *
     * @return null
     */
    public function set_click_pred_pos($value)
    {
        return $this->set(self::CLICK_PRED_POS, $value);
    }

    /**
     * Returns value of 'click_pred_pos' property
     *
     * @return int
     */
    public function get_click_pred_pos()
    {
        return $this->get(self::CLICK_PRED_POS);
    }

    /**
     * Sets value of 'card_q' property
     *
     * @param float $value Property value
     *
     * @return null
     */
    public function set_card_q($value)
    {
        return $this->set(self::CARD_Q, $value);
    }

    /**
     * Returns value of 'card_q' property
     *
     * @return float
     */
    public function get_card_q()
    {
        return $this->get(self::CARD_Q);
    }

    /**
     * Sets value of 'sortFactor' property
     *
     * @param SortFactor $value Property value
     *
     * @return null
     */
    public function set_sortFactor(SortFactor $value)
    {
        return $this->set(self::SORTFACTOR, $value);
    }

    /**
     * Returns value of 'sortFactor' property
     *
     * @return SortFactor
     */
    public function get_sortFactor()
    {
        return $this->get(self::SORTFACTOR);
    }

    /**
     * Returns value of 'sortFactor' property
     *
     * @return SortFactor
     */
    public function get_sortFactorObject()
    {
        $value = $this->get(self::SORTFACTOR);
        $instance = new SortFactor();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Sets value of 'personal_eoff' property
     *
     * @param float $value Property value
     *
     * @return null
     */
    public function set_personal_eoff($value)
    {
        return $this->set(self::PERSONAL_EOFF, $value);
    }

    /**
     * Appends value to 'personal_eoff' list
     *
     * @param float $value Value to append
     *
     * @return null
     */
    public function append_personal_eoff($value)
    {
        return $this->append(self::PERSONAL_EOFF, $value);
    }

    /**
     * Clears 'personal_eoff' list
     *
     * @return null
     */
    public function clear_personal_eoff()
    {
        return $this->clear(self::PERSONAL_EOFF);
    }

    /**
     * Returns 'personal_eoff' list
     *
     * @return float[]
     */
    public function get_personal_eoff()
    {
        return $this->get(self::PERSONAL_EOFF);
    }

    /**
     * Returns 'personal_eoff' iterator
     *
     * @return ArrayIterator
     */
    public function get_personal_eoffIterator()
    {
        return new ArrayIterator($this->get(self::PERSONAL_EOFF));
    }

    /**
     * Returns element from 'personal_eoff' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return float
     */
    public function get_personal_eoffAt($offset)
    {
        return $this->get(self::PERSONAL_EOFF, $offset);
    }

    /**
     * Returns count of 'personal_eoff' list
     *
     * @return int
     */
    public function get_personal_eoffCount()
    {
        return $this->count(self::PERSONAL_EOFF);
    }

    /**
     * Sets value of 'showgw' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_showgw($value)
    {
        return $this->set(self::SHOWGW, $value);
    }

    /**
     * Returns value of 'showgw' property
     *
     * @return bool
     */
    public function get_showgw()
    {
        return $this->get(self::SHOWGW);
    }

    /**
     * Sets value of 'yunying_merge_shia' property
     *
     * @param bool $value Property value
     *
     * @return null
     */
    public function set_yunying_merge_shia($value)
    {
        return $this->set(self::YUNYING_MERGE_SHIA, $value);
    }

    /**
     * Returns value of 'yunying_merge_shia' property
     *
     * @return bool
     */
    public function get_yunying_merge_shia()
    {
        return $this->get(self::YUNYING_MERGE_SHIA);
    }
}

/**
 * Info message
 */
class Info extends ProtobufMessage
{
    /* Field index constants */
    const QUERYINFO = 1;
    const RESULTINFO = 2;
    const UAPINFO = 3;

    /* @var string class type */
    protected static $class_type = "Info";

    /* @var array Field descriptors */
    protected static $fields = array(
        self::QUERYINFO => array(
            'name' => 'queryInfo',
            'required' => false,
            'type' => 'QueryInfo'
        ),
        self::RESULTINFO => array(
            'name' => 'resultInfo',
            'repeated' => true,
            'type' => 'NodeInfo'
        ),
        self::UAPINFO => array(
            'name' => 'uapInfo',
            'repeated' => true,
            'type' => 'uap_result_item_t'
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
        $this->values[self::QUERYINFO] = null;
        $this->values[self::RESULTINFO] = array();
        $this->values[self::UAPINFO] = array();
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
     * Sets value of 'queryInfo' property
     *
     * @param QueryInfo $value Property value
     *
     * @return null
     */
    public function set_queryInfo(QueryInfo $value)
    {
        return $this->set(self::QUERYINFO, $value);
    }

    /**
     * Returns value of 'queryInfo' property
     *
     * @return QueryInfo
     */
    public function get_queryInfo()
    {
        return $this->get(self::QUERYINFO);
    }

    /**
     * Returns value of 'queryInfo' property
     *
     * @return QueryInfo
     */
    public function get_queryInfoObject()
    {
        $value = $this->get(self::QUERYINFO);
        $instance = new QueryInfo();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Sets value of 'resultInfo' property
     *
     * @param NodeInfo $value Property value
     *
     * @return null
     */
    public function set_resultInfo($value)
    {
        return $this->set(self::RESULTINFO, $value);
    }

    /**
     * Appends value to 'resultInfo' list
     *
     * @param NodeInfo $value Value to append
     *
     * @return null
     */
    public function append_resultInfo(NodeInfo $value)
    {
        return $this->append(self::RESULTINFO, $value);
    }

    /**
     * Clears 'resultInfo' list
     *
     * @return null
     */
    public function clear_resultInfo()
    {
        return $this->clear(self::RESULTINFO);
    }

    /**
     * Returns 'resultInfo' list
     *
     * @return NodeInfo[]
     */
    public function get_resultInfo()
    {
        return $this->get(self::RESULTINFO);
    }

    /**
     * Returns 'resultInfo' iterator
     *
     * @return ArrayIterator
     */
    public function get_resultInfoIterator()
    {
        return new ArrayIterator($this->get(self::RESULTINFO));
    }

    /**
     * Returns element from 'resultInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return NodeInfo
     */
    public function get_resultInfoAt($offset)
    {
        return $this->get(self::RESULTINFO, $offset);
    }

    /**
     * Returns element from 'resultInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return NodeInfo
     */
    public function get_resultInfoObjectAt($offset)
    {
        $value = $this->get(self::RESULTINFO, $offset);
        $instance = new NodeInfo();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'resultInfo' list
     *
     * @return int
     */
    public function get_resultInfoCount()
    {
        return $this->count(self::RESULTINFO);
    }

    /**
     * Sets value of 'uapInfo' property
     *
     * @param uap_result_item_t $value Property value
     *
     * @return null
     */
    public function set_uapInfo($value)
    {
        return $this->set(self::UAPINFO, $value);
    }

    /**
     * Appends value to 'uapInfo' list
     *
     * @param uap_result_item_t $value Value to append
     *
     * @return null
     */
    public function append_uapInfo(uap_result_item_t $value)
    {
        return $this->append(self::UAPINFO, $value);
    }

    /**
     * Clears 'uapInfo' list
     *
     * @return null
     */
    public function clear_uapInfo()
    {
        return $this->clear(self::UAPINFO);
    }

    /**
     * Returns 'uapInfo' list
     *
     * @return uap_result_item_t[]
     */
    public function get_uapInfo()
    {
        return $this->get(self::UAPINFO);
    }

    /**
     * Returns 'uapInfo' iterator
     *
     * @return ArrayIterator
     */
    public function get_uapInfoIterator()
    {
        return new ArrayIterator($this->get(self::UAPINFO));
    }

    /**
     * Returns element from 'uapInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return uap_result_item_t
     */
    public function get_uapInfoAt($offset)
    {
        return $this->get(self::UAPINFO, $offset);
    }

    /**
     * Returns element from 'uapInfo' list at given offset
     *
     * @param int $offset Position in list
     *
     * @return uap_result_item_t
     */
    public function get_uapInfoObjectAt($offset)
    {
        $value = $this->get(self::UAPINFO, $offset);
        $instance = new uap_result_item_t();
        $instance->copy($value);
        return $instance;
    }

    /**
     * Returns count of 'uapInfo' list
     *
     * @return int
     */
    public function get_uapInfoCount()
    {
        return $this->count(self::UAPINFO);
    }
}
class AutoAdjustService extends PhpRpcServiceStub
{
    private function Registerauto_adjust()
    {

        $query_url_features_t_instance = New query_url_features_t();


        $click_features_t_instance = New click_features_t();

        $click_features_t_instance->append_query_url_feas($query_url_features_t_instance);
        $click_features_t_instance->append_more_query_url_feas($query_url_features_t_instance);
        $click_features_t_instance->append_time_query_url_feas($query_url_features_t_instance);

        $QueryInfo_instance = New QueryInfo();

        $QueryInfo_instance->set_click_features($click_features_t_instance);

        $offset_t_instance = New offset_t();


        $SortInfo_instance = New SortInfo();


        $SortFactor_instance = New SortFactor();


        $NodeInfo_instance = New NodeInfo();

        $NodeInfo_instance->set_offsetInfo($offset_t_instance);
        $NodeInfo_instance->set_sortInfo($SortInfo_instance);
        $NodeInfo_instance->set_sortFactor($SortFactor_instance);

        $uap_result_item_t_instance = New uap_result_item_t();


        $Info_instance = New Info();

        $Info_instance->set_queryInfo($QueryInfo_instance);
        $Info_instance->append_resultInfo($NodeInfo_instance);
        $Info_instance->append_uapInfo($uap_result_item_t_instance);

        $OutNodeInfo_instance = New OutNodeInfo();


        $OutInfo_instance = New OutInfo();

        $OutInfo_instance->append_resultInfo($OutNodeInfo_instance);

        $this->RegisterMethod("auto_adjust", $Info_instance, $OutInfo_instance);
    }

    function __construct($server_address)
    {
        $this->InitService($server_address, "sofa.pbrpc.vui", "AutoAdjustService");
        $this->Registerauto_adjust();
        $this->InitMethods();
    }

    public function auto_adjust($request, $response, $closure)
    {
        $this->CallMethod("auto_adjust", $request, $response, $closure);
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

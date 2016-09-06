<?php
/**
 * Describes enum value
 */
class EnumValueDescriptor
{
    private $_name;
    private $_value;

    /**
     * Constructs new enum value descriptor
     *
     * @param string $name  Name
     * @param string $value Value
     */
    public function __construct($name, $value)
    {
        $this->_name = $name;
        $this->_value = $value;
    }

    /**
     * Returns name
     *
     * @return string
     */
    public function getName()
    {
        return $this->_name;
    }

    /**
     * Returns value
     *
     * @return string
     */
    public function getValue()
    {
        return $this->_value;
    }
}

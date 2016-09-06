<?php
/**
 * Describes enum
 */
class EnumDescriptor implements DescriptorInterface
{
    private $_containing;
    private $_file;
    private $_name;
    private $_values = array();

    /**
     * Creates new enum descriptor
     *
     * @param string            $name       Enum name
     * @param FileDescriptor    $file       Containing file
     * @param MessageDescriptor $containing Containing message
     */
    public function __construct(
        $name, FileDescriptor $file, $containing
    ) {
        $this->_name = $name;
        $this->_file = $file;
        $this->_containing = $containing;

        if (is_null($containing)) {
            $file->addEnum($this);
        }

        if (!is_null($containing)) {
            $containing->addEnum($this);
        }
    }

    /**
     * Adds new enum value
     *
     * @param EnumValueDescriptor $value Value
     *
     * @return null
     */
    public function addValue(EnumValueDescriptor $value)
    {
        $this->_values[] = $value;
    }

    /**
     * Returns containing message
     *
     * @return MessageDescriptor
     */
    public function getContaining()
    {
        return $this->_containing;
    }

    /**
     * Returns containing file
     *
     * @return FileDescriptor
     */
    public function getFile()
    {
        return $this->_file;
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
     * Returns enum values descriptors
     *
     * @return EnumValueDescriptor[]
     */
    public function getValues()
    {
        return $this->_values;
    }
}

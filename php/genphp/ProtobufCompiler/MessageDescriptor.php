<?php
/**
 * MessageDescriptor encapsulates protobuf message description
 */
class MessageDescriptor implements DescriptorInterface
{
    private $_containing;
    private $_enums = array();
    private $_fields = array();
    private $_file;
    private $_name;
    private $_nested = array();
    private $_types = array();

    /**
     * Constructs new message descriptor
     *
     * @param string            $name       Message name
     * @param FileDescriptor    $file       File defining message
     * @param MessageDescriptor $containing Parent message
     */
    public function __construct(
        $name, FileDescriptor $file, MessageDescriptor $containing = null
    ) {
        $this->_name = $name;
        $this->_file = $file;
        $this->_containing = $containing;

        if (is_null($containing)) {
            $file->addMessage($this);
        }

        if (!is_null($containing)) {
            $containing->addNested($this);
        }
    }

    /**
     * Adds new enum to message
     *
     * @param EnumDescriptor $enum Enum
     *
     * @return null
     */
    public function addEnum(EnumDescriptor $enum)
    {
        $this->_enums[$enum->getName()] = $enum;
    }

    /**
     * Adds new field to message
     *
     * @param FieldDescriptor $field Field
     *
     * @return null
     */
    public function addField(FieldDescriptor $field)
    {
        $this->_fields[] = $field;
    }

    /**
     * Adds nested message
     *
     * @param MessageDescriptor $nested Message
     *
     * @return null
     */
    public function addNested(MessageDescriptor $nested)
    {
        $this->_nested[$nested->getName()] = $nested;
    }

    /**
     * Finds type in message for given name
     *
     * @param string $name      Name to search for
     * @param string $namespace Namespace to search in
     *
     * @return DescriptorInterface|boolean
     */
    public function findType($name, $namespace = null)
    {
        $containing = $this;

        if (!is_null($namespace)) {
            $namespace = explode('.', $namespace);

            foreach ($namespace as $n) {
                if (!isset($containing->_nested[$n])) {
                    return false;
                } else {
                    $containing = $containing->_nested[$n];
                }
            }
        }

        if (isset($containing->_nested[$name])) {
            return $containing->_nested[$name];
        }

        if (isset($containing->_enums[$name])) {
            return $containing->_enums[$name];
        }

        return false;
    }

    /**
     * Returns parent message
     *
     * @return MessageDescriptor
     */
    public function getContaining()
    {
        return $this->_containing;
    }

    /**
     * Returns defined enums
     *
     * @return EnumDescriptor[]
     */
    public function getEnums()
    {
        return $this->_enums;
    }

    /**
     * Returns defined messages
     *
     * @return FieldDescriptor[]
     */
    public function getFields()
    {
        return $this->_fields;
    }

    /**
     * Returns file descriptor
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
     * Returns sub-messages
     *
     * @return array
     */
    public function getNested()
    {
        return $this->_nested;
    }
}

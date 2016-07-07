<?php
/**
 * Describes field
 */
class FieldDescriptor
{
    private static $_scalarTypes = array(
        'double'   => \ProtobufMessage::PB_TYPE_DOUBLE,
        'float'    => \ProtobufMessage::PB_TYPE_FLOAT,
        'int32'    => \ProtobufMessage::PB_TYPE_INT,
        'int64'    => \ProtobufMessage::PB_TYPE_INT,
        'uint32'   => \ProtobufMessage::PB_TYPE_INT,
        //'uint64'   => \ProtobufMessage::PB_TYPE_UINT64,
        'sint32'   => \ProtobufMessage::PB_TYPE_SIGNED_INT,
        'sint64'   => \ProtobufMessage::PB_TYPE_SIGNED_INT,
        'fixed32'  => \ProtobufMessage::PB_TYPE_FIXED32,
        'fixed64'  => \ProtobufMessage::PB_TYPE_FIXED64,
        'sfixed32' => \ProtobufMessage::PB_TYPE_FIXED32,
        'sfixed64' => \ProtobufMessage::PB_TYPE_FIXED64,
        'bool'     => \ProtobufMessage::PB_TYPE_BOOL,
        'string'   => \ProtobufMessage::PB_TYPE_STRING,
        'bytes'    => \ProtobufMessage::PB_TYPE_STRING,
    );

    private static $_scalarNativeTypes = array(
        'double'   => 'float',
        'float'    => 'float',
        'int32'    => 'int',
        'int64'    => 'int',
        'uint32'   => 'int',
        'uint64'   => 'string',
        'sint32'   => 'int',
        'sint64'   => 'int',
        'fixed32'  => 'int',
        'fixed64'  => 'int',
        'sfixed32' => 'int',
        'sfixed64' => 'int',
        'bool'     => 'bool',
        'string'   => 'string',
        'bytes'    => 'string',
    );

    private $_default;
    private $_label;
    private $_name;
    private $_namespace = null;
    private $_number;
    private $_type;
    private $_typeDescriptor = null;

    /**
     * Returns default value
     *
     * @return string
     */
    public function getDefault()
    {
        return $this->_default;
    }

    /**
     * Returns label
     *
     * @return string
     */
    public function getLabel()
    {
        return $this->_label;
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
     * Returns camel case name
     *
     * @return string
     */
    public function getCamelCaseName()
    {
        //$chunks = preg_split('/[^a-z0-9]/is', $this->getName());
        //return implode('', array_map('ucfirst', $chunks));
        return $this->getName();
    }

    /**
     * Returns const name
     *
     * @return string
     */
    public function getConstName()
    {
        $chunks = preg_split('/[^a-z0-9]/is', $this->getName());
        return implode('_', array_map('strtoupper', $chunks));
    }

    /**
     * Returns namespace
     *
     * @return string
     */
    public function getNamespace()
    {
        return $this->_namespace;
    }

    /**
     * Returns number
     *
     * @return int
     */
    public function getNumber()
    {
        return $this->_number;
    }

    /**
     * Returns scalar type
     *
     * @return int
     */
    public function getScalarType()
    {
        return self::$_scalarTypes[strtolower($this->_type)];
    }

    /**
     * Returns PHP type
     *
     * @return string
     */
    public function getTypeName()
    {
        if (isset(self::$_scalarNativeTypes[strtolower($this->_type)])) {
            return self::$_scalarNativeTypes[strtolower($this->_type)];
        } else {
            if ($this->_typeDescriptor instanceof \EnumDescriptor) {
                return 'int';
            } else {
                return $this->_type;
            }
        }
    }

    /**
     * Returns true if is native type
     *
     * @return bool
     */
    public function isScalarType()
    {
        return isset(self::$_scalarNativeTypes[strtolower($this->_type)]);
    }

    /**
     * Returns type
     *
     * @return type
     */
    public function getType()
    {
        return $this->_type;
    }

    /**
     * Returns type descriptor
     *
     * @return string
     */
    public function getTypeDescriptor()
    {
        return $this->_typeDescriptor;
    }

    /**
     * Returns true if field is repeated
     *
     * @return bool
     */
    public function isRepeated()
    {
        return $this->_label == FieldLabel::REPEATED;
    }

    /**
     * Returns true if is requireg
     *
     * @return bool
     */
    public function isRequired()
    {
        return $this->_label == FieldLabel::REQUIRED;
    }

    /**
     * Returns true if is scalar
     *
     * @return bool
     */
    public function isProtobufScalarType()
    {
        return isset(self::$_scalarTypes[strtolower($this->_type)]);
    }

    /**
     * Returns true if is optional
     *
     * @return bool
     */
    public function isOptional()
    {
        return $this->_label == FieldLabel::OPTIONAL;
    }

    /**
     * Sets default value
     *
     * @param mixed $default Default value
     *
     * @return null
     */
    public function setDefault($default)
    {
        $this->_default = $default;
    }

    /**
     * Sets label
     *
     * @param string $label Label
     *
     * @return null
     */
    public function setLabel($label)
    {
        $this->_label = $label;
    }

    /**
     * Sets name
     *
     * @param string $name Name
     *
     * @return null
     */
    public function setName($name)
    {
        $this->_name = $name;
    }

    /**
     * Sets namespace
     *
     * @param string $namespace Namespace
     *
     * @return null
     */
    public function setNamespace($namespace)
    {
        $this->_namespace = $namespace;
    }

    /**
     * Sets number
     *
     * @param int $number Number
     *
     * @return null
     */
    public function setNumber($number)
    {
        $this->_number = $number;
    }

    /**
     * Sets type
     *
     * @param int $type Type
     *
     * @return null
     */
    public function setType($type)
    {
        $this->_type = $type;
    }

    /**
     * Sets type descriptor
     *
     * @param int $typeDescriptor Type descriptor
     *
     * @return null
     */
    public function setTypeDescriptor($typeDescriptor)
    {
        $this->_typeDescriptor = $typeDescriptor;
    }
}

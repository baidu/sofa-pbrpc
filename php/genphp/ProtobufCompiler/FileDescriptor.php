<?php
/**
 * Describes file
 */
class FileDescriptor
{
    /** @var FileDescriptor[] */
    private $_dependencies = array();

    /** @var EnumDescriptor[] */
    private $_enums = array();

    /** @var MessageDescriptor[] */
    private $_messages = array();

    /** @var ServiceDescriptor */
    private $_service;

    /** @var string */
    private $_name;

    /** @var string */
    private $_package;

    /**
     * Constructs new file descriptor
     *
     * @param string $name Name
     */
    public function __construct($name)
    {
        $this->_name = $name;
    }

    /**
     * Adds new dependency
     *
     * @param FileDescriptor $dependency Depending file
     *
     * @return null
     */
    public function addDependency(FileDescriptor $dependency)
    {
        $this->_dependencies[] = $dependency;
    }

    /**
     * Adds new enum
     *
     * @param EnumDescriptor $enum Enum
     *
     * @return null
     */
    public function addEnum(EnumDescriptor $enum)
    {
        $this->_enums[] = $enum;
    }

    /**
     * Adds new message
     *
     * @param MessageDescriptor $message Message
     *
     * @return null
     */
    public function addMessage(MessageDescriptor $message)
    {
        $this->_messages[$message->getName()] = $message;
    }

    /**
     * Adds new service
     *
     * @param ServiceDescriptor $service
     *
     * @return null
     */
    public function addService(ServiceDescriptor $service)
    {
        $this->_service = $service;
    }

    /**
     * Returns required files
     *
     * @return array|FileDescriptor[]
     */
    public function getDependencies()
    {
        return $this->_dependencies;
    }

    /**
     * Returns defines enums
     *
     * @return array|EnumDescriptor[]
     */
    public function getEnums()
    {
        return $this->_enums;
    }

    /**
     * Returns defined messages
     *
     * @return array|MessageDescriptor[]
     */
    public function getMessages()
    {
        return $this->_messages;
    }

    /**
     *
     * @param string $name name of message
     *
     * Returns MessageDescriptor with name $name
     *
     * @return MessageDescriptor
     */
    public function findMessage($name)
    {
        return $this->_messages[$name];
    }

    /**
     *
     * @param string $name name of message
     *
     * Returns whether has mesage with name $name
     *
     * @return Boolean
     */
    public function hasMessage($name)
    {
        return array_key_exists($name, $this->_messages);
    }

    /**
     * Returns defined service 
     *
     * @return service 
     */
    public function getService()
    {
        return $this->_service;
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
     * Returns package
     *
     * @return string
     */
    public function getPackage()
    {
        return $this->_package;
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
     * Sets package name
     *
     * @param string $package Package
     *
     * @return null
     */
    public function setPackage($package)
    {
        $this->_package = $package;
    }
}

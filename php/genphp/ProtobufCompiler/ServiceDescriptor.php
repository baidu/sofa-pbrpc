<?php

/**
 * generates service class
 */

class ServiceDescriptor implements DescriptorInterface
{
    private $_containing;
    private $_methods = array();
    private $_file;
    private $_name;

    /**
     * Constructs new message descriptor
     *
     * @param string            $name       Message name
     * @param FileDescriptor    $file       File defining message
     * @param MessageDescriptor $containing Parent message
     */
    public function __construct(
        $name, FileDescriptor $file) {
        $this->_name = $name;
        $this->_file = $file;

        if (is_null($file)) {
            throw new Exception(
                'file descriptor cannot be null'
            );
        }
        if (!is_null($file)) {
            $file->addService($this);
        }
    }

    /**
     * Returns file descriptor 
     *
     * @return FileDescriptorDescriptor
     */
    public function getContaining()
    {
        return $this->_file;
    }

    /**
     * Adds new method to service
     *
     * @param MethodDescriptor $method
     *
     * @return null
     */
    public function addMethod(MethodDescriptor $method)
    {
        $this->_methods[$method->getName()] = $method;
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
     * Returns get method descriptor
     *
     * @return MethodDescriptor
     */
    public function getMethods()
    {
        return $this->_methods;
    } 
}

?>

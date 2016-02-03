<?php

/**
 * generates Method interface
 */

class MethodDescriptor implements DescriptorInterface
{
    private $_containing;
    private $_input;
    private $_output;
    private $_service;
    private $_file;
    private $_name;

    /**
     * Constructs new message descriptor
     *
     * @param string            $name       Message name
     * @param string            $input      input type
     * @param string            $output     output type
     * @param FileDescriptor    $file       File defining message
     * @param ServiceDescriptor $service    service descriptor
     */
    public function __construct(
        $name, $input, $output, FileDescriptor $file, ServiceDescriptor $service) {
        $this->_name = $name;
        $this->_file = $file;
        $this->_input = $input;
        $this->_output = $output;
        //$this->_containing = $containing;

        if (is_null($service)) {
            throw new Exception(
                    'Service cannot be null'
                    );
        }
        if (!is_null($service)) {
            $service->addMethod($this);
        }
    }

    /**
     * Returns parent message
     *
     * @return ServiceDescriptor
     */
    public function getContaining()
    {
        return $this->_service;
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
     * Returns service descriptor
     *
     * @return ServiceDescriptor
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
     * Returns input type
     *
     * @return string
     */
    public function getInput()
    {
        return $this->_input;
    }

    /**
     * Returns output type
     *
     * @return string
     */
    public function getOutput()
    {
        return $this->_output;
    }
}

?>

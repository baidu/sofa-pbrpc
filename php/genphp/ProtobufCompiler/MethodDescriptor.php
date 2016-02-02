<?php
/* * *****************************************************************************
* 
* Copyright (c) 2015 Baidu.com, Inc. All Rights Reserved
* 
 * **************************************************************************** */
 
/**
* @File: MethodDescriptor.php
* @Author: spider(spider@baidu.com)
* @Date: 2015/11/29 14:34:44
* @Brief: method descriptor for rpc     
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
     * @param FileDescriptor    $file       File defining message
     * @param MessageDescriptor $containing Parent message
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
     * @return MessageDescriptor
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
     * Returns file descriptor
     *
     * @return FileDescriptor
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

<?php
interface DescriptorInterface
{
    /**
     * Returns name
     *
     * @return string
     */
    public function getName();

    /**
     * Returns parent message
     *
     * @return string
     */
    public function getContaining();

    /**
     * Returns parent file
     *
     * @return FileDescriptor
     */
    public function getFile();
}

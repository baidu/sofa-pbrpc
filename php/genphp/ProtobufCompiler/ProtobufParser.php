<?php
require_once 'DescriptorInterface.php';
require_once 'MessageDescriptor.php';
require_once 'FileDescriptor.php';
require_once 'FieldDescriptor.php';
require_once 'FieldLabel.php';
require_once 'EnumDescriptor.php';
require_once 'EnumValueDescriptor.php';
require_once 'CodeStringBuffer.php';
require_once 'CommentStringBuffer.php';
require_once 'ServiceDescriptor.php';
require_once 'MethodDescriptor.php';

/**
 * Parses protobuf file and generates message class
 */
class ProtobufParser
{
    const NAMESPACE_SEPARATOR = '_';
    const NAMESPACE_SEPARATOR_NATIVE = '\\';
    const SERVICE_CLASS = "PhpRpcServiceStub";

    private static $_globalNamespace = '';
    private static $_parsers = array();
    private $_file = null;
    private $_namespaces = array();
    private static $_finished = array();

    const TAB = '    ';
    const EOL = PHP_EOL;

    private $_hasSplTypes = false;
    private $_useNativeNamespaces = false;

    public function __construct($useNativeNamespaces = null)
    {
        $this->_hasSplTypes = extension_loaded('SPL_Types');
        $this->_useNativeNamespaces = (boolean)$useNativeNamespaces;
    }

    /**
     * Returns namespaces
     *
     * @return array
     */
    public function getNamespaces()
    {
        return $this->_namespaces;
    }

    /**
     * Returns namespace separator
     *
     * @return array
     */
    public function getNamespaceSeparator()
    {
        return $this->_useNativeNamespaces
            ? self::NAMESPACE_SEPARATOR_NATIVE : self::NAMESPACE_SEPARATOR;
    }

    /**
     * Creates package name based on proto package name value
     *
     * @param string $name Package name
     *
     * @return string
     */
    public function createPackageName($name)
    {
        $components = explode('.', $name);
        $name = '';

        foreach ($components as $component) {
            if (strlen($component) > 0) {
                $name .= $this->getNamespaceSeparator() . ucfirst($component);
            }
        }

        return trim($name, $this->getNamespaceSeparator());
    }

    /**
     * Created type name based on proto type name
     *
     * @param string $name Type name
     *
     * @return string
     */
    public static function createTypeName($name)
    {
        $components = explode('_', $name);
        $name = '';

        foreach ($components as $component) {
            if (strlen($component) > 0) {
                $name .= ucfirst($component);
            }
        }

        return $name;
    }

    /**
     * Generates message files (PHP code) for proto file
     *
     * @param string $sourceFile Source filename (.proto)
     * @param string $outputFile Output filename
     *
     * @return null
     */
    public function parse($sourceFile, $outputFile = null)
    {
        $string = file_get_contents($sourceFile);
        $this->_file = new FileDescriptor($sourceFile);
        $this->_stripComments($string);

        $string = trim($string);

        $file = new FileDescriptor($sourceFile);
        $this->_parseMessageType($file, $string);

        $this->_resolveNamespaces($file);
        $buffer = new CodeStringBuffer(self::TAB, self::EOL);
        $this->_createClassFile($file, $buffer, $outputFile);

        return $file;
    }

    private function _findFieldName($fields, $type)
    {
        $field_names = array();
        foreach ($fields as $index => $descriptor)
        {
            if ($descriptor["type"] == $type)
            {
                $field_names[] = $descriptor["name"];
            } 
        }
        return $field_names;
    }

    private function _createParaConstruct(FileDescriptor $file, MessageDescriptor $msg, CodeStringBuffer $buffer)
    {
        $finished_type = self::$_finished;
        if(in_array($msg->getName(), $finished_type))
        {
            return;
        }
        $fields = $msg->getFields();
        $set_content = "";
        $parent_instance = "\$" . $msg->getName() . "_instance";
        $new_parent = str_repeat(' ', 8) . $parent_instance . " = New " . $msg->getName() . "();\n";
        
        foreach ($fields as $index => $field)
        {
            $type = $field->getType();
            $label = $field->getLabel();
            $name = $field->getName();
            if ($file->hasMessage($type))
            {
                $child = $file->findMessage($type);
                $this->_createParaConstruct($file, $child, $buffer);

                //$new_content = $new_content .  "\$" . $type . "_instance = New " . $type . "();\n";
                if ($label == 1 || $label == 2)
                {
                    $set_content = $set_content . str_repeat(' ', 8) . $parent_instance . "->set_" . $name . "(\$" .  $type . "_instance" . ");\n";
                }
                else if ($label == 3)
                {
                    $set_content = $set_content . str_repeat(' ', 8) . $parent_instance . "->append_" . $name . "(\$" .  $type . "_instance" . ");\n";
                }
            } 
        }
        $finished_type = self::$_finished;
        if(!in_array($msg->getName(), $finished_type))
            $buffer->append($new_parent);
        $buffer->append($set_content);
        self::$_finished[] = $msg->getName();
    }
    /**
     * Generates method description and write it to buffer
     *
     * @param MethodDescriptor $descriptor Method descriptor
     * @param CodeStringBuffer  $buffer     Buffer to write code to
     *
     * @return null
     */
    private function _createRegisterMethod(FileDescriptor $file, MethodDescriptor $method, CodeStringBuffer $buffer)
    {
        //unset(self::$_finished);
        self::$_finished = array();
        $name = $method->getName();
        $input_type = $method->getInput();
        $output_type = $method->getOutput();
        $input_descriptor = $file->findMessage($input_type);
        $output_descriptor = $file->findMessage($output_type); 

        $input_fields = $input_descriptor->getFields();
        $output_fields = $output_descriptor->getFields();

        $content = '' . str_repeat(' ', 4) . 'private function Register' . $name . '()' . "\n";
        $content = $content . str_repeat(" ", 4) . "{\n";
        $buffer->append($content);
        $content = "";

        $this->_createParaConstruct($file, $input_descriptor, $buffer); 
        $this->_createParaConstruct($file, $output_descriptor, $buffer); 

        $request_instance = "\$" . $input_type . "_instance";
        $response_instance = "\$" . $output_type . "_instance";
        //$content = $content . str_repeat(" ", 8) . "\$request = new " . $input_type . "();\n";
        //$content = $content . str_repeat(" ", 8) . "\$response = new " . $output_type . "();\n";
        $content = $content . str_repeat(" ", 8) . "\$this->RegisterMethod(\"" . $name . "\", " . $request_instance . ", " .  $response_instance. ");\n";
        $content = $content . str_repeat(" ", 4) . "}\n";
        $buffer->append($content);
        return $name;
    }

    private function _createUserMethod(MethodDescriptor $method, CodeStringBuffer $buffer) 
    {
        $name = $method->getName();
        $content = '' . str_repeat(' ', 4) . 'public function ' . $name . '($request, $response, $closure)' . "\n";
        $content = $content . str_repeat(' ', 4) . "{\n";
        $content = $content . str_repeat(' ', 8) . '$this->CallMethod("' .$name . '", $request, $response, $closure);';
        $content = $content . "\n" . str_repeat(' ', 4) . "}\n";
        $buffer->append($content);
    }

    private function _createServiceCommonMethod(CodeStringBuffer $buffer) 
    {
        $content = '' . str_repeat(' ', 4) . 'public function Failed()' . "\n";
        $content = $content . str_repeat(' ', 4) . "{\n";
        $content = $content . str_repeat(' ', 8) . 'return $this->GetFailed();';
        $content = $content . "\n" . str_repeat(' ', 4) . "}\n";
        $buffer->append($content);

        $content = '' . str_repeat(' ', 4) . 'public function ErrorText()' . "\n";
        $content = $content . str_repeat(' ', 4) . "{\n";
        $content = $content . str_repeat(' ', 8) . 'return $this->GetErrorText();';
        $content = $content . "\n" . str_repeat(' ', 4) . "}\n";
        $buffer->append($content);
    }


    /**
     * Generates class description and write it to buffer
     *
     * @param FileDescriptor $descriptor  File descriptor
     * @param ServiceDescriptor $descriptor  Service descriptor
     * @param CodeStringBuffer  $buffer  Buffer to write code to
     *
     * @return null
     */
    private function _createService(FileDescriptor $file,  ServiceDescriptor $descriptor, CodeStringBuffer $buffer) 
    {
        $name = $descriptor->getName();
        $class_frame = "class " . $name . " extends " . self::SERVICE_CLASS . "\n{";
        $buffer->append($class_frame);
        $register_content = "";
        $content = str_repeat(' ', 4) . 'function __construct($server_address)' . "\n";
        $content = $content . str_repeat(' ', 4) . "{\n";
        foreach ($descriptor->getMethods() as $method) {
            $func_name = $this->_createRegisterMethod($file, $method, $buffer);
            $func_name = "Register" . $func_name;
            $register_content = $register_content . str_repeat(' ', 8) . "\$this->" . $func_name . "();\n";
        }
        $content = $content . str_repeat(' ', 8) . '$this->InitService($server_address, "' . $file->getPackage() . '", "' . $descriptor->getName() . "\");\n";
        $content = $content . $register_content;
        $content = $content . str_repeat(' ', 8) . "\$this->InitMethods();\n";
        $content = $content . str_repeat(' ', 4) . "}\n";
        $buffer->append($content);
        foreach ($descriptor->getMethods() as $method) {
            $this->_createUserMethod($method, $buffer);
            $this->_createServiceCommonMethod($buffer);
        }
        $buffer->append('}');
    
    }
    /**
     * Generates class description and write it to buffer
     *
     * @param MessageDescriptor $descriptor Message descriptor
     * @param CodeStringBuffer  $buffer     Buffer to write code to
     *
     * @return null
     */
    private function _createClass(
        MessageDescriptor $descriptor, CodeStringBuffer $buffer
    ) {
        foreach ($descriptor->getEnums() as $enum) {
            $this->_createEnum($enum, $buffer);
        }

        foreach ($descriptor->getNested() as $nested) {
            $this->_createClass($nested, $buffer);
        }

        $buffer->newline();

        if ($this->_useNativeNamespaces) {
            $name = self::createTypeName($descriptor->getName());
            $namespaceName = $this->_createNamespaceName($descriptor);
            $buffer->append('namespace ' . $namespaceName . ' {');
        } else {
            $name = $this->_createClassName($descriptor);
        }

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $path = $this->_createEmbeddedMessagePath($descriptor);
        if ($path) {
            $comment->append($descriptor->getName() . ' message embedded in ' . $path . ' message');
        } else {
            $comment->append($descriptor->getName() . ' message');
        }

        $buffer->append($comment);

        $buffer->append('class ' . $name . ' extends ProtobufMessage')
            ->append('{')
            ->increaseIdentation();

        $this->_createClassConstructor($descriptor->getName(), $descriptor->getFields(), $buffer);
        $this->_createClassBody($descriptor->getFields(), $buffer);

        $buffer->decreaseIdentation()
            ->append('}');

        if ($this->_useNativeNamespaces) {
            $buffer->append('}');
        }
    }

    /**
     * Creates enum description
     *
     * @param EnumDescriptor   $descriptor Enum descriptor
     * @param CodeStringBuffer $buffer     Buffer to write code to
     *
     * @return null
     */
    private function _createEnum(
        EnumDescriptor $descriptor, CodeStringBuffer $buffer
    ) {
        $buffer->newline();

        if ($this->_useNativeNamespaces) {
            $name = self::createTypeName($descriptor->getName());
            $namespaceName = $this->_createNamespaceName($descriptor);
            $buffer->append('namespace ' . $namespaceName . ' {');
        } else {
            $name = $this->_createClassName($descriptor);
        }

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $path = $this->_createEmbeddedMessagePath($descriptor);
        if ($path) {
            $comment->append($descriptor->getName() . ' enum embedded in ' . $path . ' message');
        } else {
            $comment->append($descriptor->getName() . ' enum');
        }

        $buffer->append($comment);

        if ($this->_hasSplTypes) {
            $buffer->append('final class ' . $name .' extends SplEnum')
                ->append('{')
                ->increaseIdentation();
        } else {
            $buffer->append('final class ' . $name)
                ->append('{')
                ->increaseIdentation();
        }

        $this->_createEnumClassDefinition($descriptor->getValues(), $buffer);

        $buffer->decreaseIdentation()
            ->append('}');

        if ($this->_useNativeNamespaces) {
            $buffer->append('}');
        }
    }

    /**
     * Creates class code for given file descriptor
     *
     * @param FileDescriptor   $file       File descriptor
     * @param CodeStringBuffer $buffer     Buffer to write code to
     * @param string|null      $outputFile Output filename
     *
     * @return null
     */
    private function _createClassFile(
        FileDescriptor $file, CodeStringBuffer $buffer, $outputFile = null
    ) {
        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $date = strftime("%Y-%m-%d %H:%M:%S");
        $comment->append('Auto generated from ' . basename($file->getName()) . ' at ' . $date);

        if ($file->getPackage()) {
            $comment->newline()
                ->append($file->getPackage() . ' package');
        }

        $buffer->append($comment);

        foreach ($file->getEnums() as $name => $descriptor) {
            $this->_createEnum($descriptor, $buffer);
        }

        foreach ($file->getMessages() as $name => $descriptor) {
            $this->_createClass($descriptor, $buffer);
        }

        if (($descriptor = $file->getService())) {
            $this->_createService($file, $descriptor, $buffer);
        }

        $requiresString = '';

        foreach ($file->getDependencies() as $dependency) {
            $requiresString .= sprintf(
                'require_once \'%s\';',
                $this->_createOutputFilename($dependency->getName())
            );
        }

        if ($this->_useNativeNamespaces && !empty($requiresString)) {
            $requiresString = 'namespace {' . PHP_EOL . $requiresString . PHP_EOL . '}';
        }

        $buffer->append($requiresString);

        if ($outputFile == null) {
            $outputFile = $this->_createOutputFilename($file->getName());
        }

        file_put_contents($outputFile, '<?php' . PHP_EOL . $buffer);
    }

    /**
     * Generates namespace name for given descriptor
     *
     * @param DescriptorInterface $descriptor Descriptor
     *
     * @return string
     */
    private function _createNamespaceName(DescriptorInterface $descriptor)
    {
        $namespace = array();

        $containing = $descriptor->getContaining();

        while (!is_null($containing)) {
            $namespace[] = self::createTypeName($containing->getName());
            $containing = $containing->getContaining();
        }

        $package = $descriptor->getFile()->getPackage();

        if (!empty($package)) {
            $namespace[] = $this->createPackageName($package);
        }

        $namespace = array_reverse($namespace);

        $name = implode($this->getNamespaceSeparator(), $namespace);

        return $name;
    }

    /**
     * Generates class name for given descriptor
     *
     * @param DescriptorInterface $descriptor Descriptor
     *
     * @return string
     */
    private function _createClassName(DescriptorInterface $descriptor)
    {
        //$name = self::createTypeName($descriptor->getName());

        //$prefix = $this->_createNamespaceName($descriptor);
        //if (!empty($prefix)) {
        //    //$name = $prefix . $this->getNamespaceSeparator() . $name;
        //    if ($this->_useNativeNamespaces) {
        //        $name = self::NAMESPACE_SEPARATOR_NATIVE . $name;
        //    }
        //}
        $name = $descriptor->getName();
        return $name;
    }

    /**
     * Generates filename for given source filename
     *
     * @param string $sourceFilename Filename
     *
     * @return string
     */
    private function _createOutputFilename($sourceFilename)
    {
        $pathInfo = pathinfo($sourceFilename);

        //return 'pb_proto_' . $pathInfo['filename'] . '.php';
        return $pathInfo['filename'] . '.pb.php';
    }

    /**
     * Creates embedded message path composed of ancestor messages
     * seperated by slash "/". If message has no ancestor returns empty string.
     *
     * @param DescriptorInterface $descriptor message descriptor
     *
     * @return string
     */
    private function _createEmbeddedMessagePath(DescriptorInterface $descriptor)
    {
        $containing = $descriptor->getContaining();
        $path = array();

        while ($containing) {
            $path[] = $containing->getName();
            $containing = $containing->getContaining();
        }

        array_reverse($path);

        return implode("/", $path);
    }

    /**
     * Generates type name for given descriptor
     *
     * @param FieldDescriptor $field Field descriptor
     *
     * @return string
     */
    private function _getType(FieldDescriptor $field)
    {
        if ($field->isProtobufScalarType()) {
            return $field->getScalarType();
        } else if ($field->getTypeDescriptor() instanceof EnumDescriptor) {
            return ProtobufMessage::PB_TYPE_INT;
        } else {
            return $this->_createClassName($field->getTypeDescriptor());
        }
    }

    /**
     * Generates class body for given field descriptors list and
     * writes it to buffer
     *
     * @param array            $fields Array of FieldDescriptors
     * @param CodeStringBuffer $buffer Buffer to write code to
     *
     * @return null
     */
    private function _createClassBody($fields, CodeStringBuffer $buffer)
    {
        $comment = new CommentStringBuffer(self::TAB, self::EOL);

        $comment->append('Returns class type')
            ->newline()
            ->appendParam('return', 'string');

        $buffer->append($comment)
            ->append('public function class_type()')
            ->append('{')
            ->append('return self::$class_type;', false, 1)
            ->append('}')
            ->append('');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);

        $comment->append('Returns field descriptors')
            ->newline()
            ->appendParam('return', 'array');

        $buffer->append($comment)
            ->append('public function fields()')
            ->append('{')
            ->append('return self::$fields;', false, 1)
            ->append('}');

        foreach ($fields as $field) {
            if ($field->isRepeated()) {
                $this->_describeRepeatedField($field, $buffer);
            } else {
                $this->_describeSingleField($field, $buffer);
            }
        }
    }

    /**
     * Describes repeated field
     *
     * @param FieldDescriptor  $field  Field descriptor
     * @param CodeStringBuffer $buffer Buffer to write to
     *
     * @return null
     */
    private function _describeRepeatedField(
        FieldDescriptor $field, CodeStringBuffer $buffer
    ) {
        if ($field->isProtobufScalarType() || $field->getTypeDescriptor() instanceof EnumDescriptor) {
            $typeName = $field->getTypeName();
            $argumentClass = '';
        } else {
            $typeName = $this->_createClassName($field->getTypeDescriptor());
            $argumentClass = $typeName . ' ';
        }
        $comment = new CommentStringBuffer(self::TAB, self::EOL);

        $comment->append(
            'Sets value of \'' . $field->getName() . '\' property'
        )
            ->newline()
            ->appendParam(
                'param',
                $typeName . ' $value Property value'
            )
            ->newline()
            ->appendParam('return', 'null');

        $buffer->newline()
            ->append($comment)
            ->append(
                'public function set_' . $field->getCamelCaseName() .
                '($value)'
            )
            ->append('{')
            ->append(
                'return $this->set(self::' .
                $field->getConstName() . ', $value);',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Appends value to \'' . $field->getName() . '\' list')
            ->newline()
            ->appendParam('param', $typeName . ' $value Value to append')
            ->newline()
            ->appendParam('return', 'null');

        $buffer->newline()
            ->append($comment)
            ->append(
                'public function append_' . $field->getCamelCaseName() . '(' . $argumentClass . '$value)'
            )
            ->append('{')
            ->append(
                'return $this->append(self::' . $field->getConstName() . ', $value);',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Clears \'' . $field->getName() . '\' list')
            ->newline()
            ->appendParam('return', 'null');

        $buffer->newline()
            ->append($comment)
            ->append('public function clear_' . $field->getCamelCaseName() . '()')
            ->append('{')
            ->append(
                'return $this->clear(self::' . $field->getConstName() . ');',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Returns \'' . $field->getName() . '\' list')
            ->newline()
            ->appendParam('return', $typeName . '[]');

        $buffer->newline()
            ->append($comment)
            ->append('public function get_' . $field->getCamelCaseName() . '()')
            ->append('{')
            ->append(
                'return $this->get(self::' . $field->getConstName() . ');',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Returns \'' . $field->getName() . '\' iterator')
            ->newline()
            ->appendParam('return', 'ArrayIterator');

        $buffer->newline()
            ->append($comment)
            ->append(
                'public function get_' . $field->getCamelCaseName() . 'Iterator()'
            )
            ->append('{')
            ->append(
                'return new ArrayIterator($this->get(self::' .
                $field->getConstName() . '));',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append(
            'Returns element from \'' . $field->getName() .
            '\' list at given offset'
        )
            ->newline()
            ->appendParam('param', 'int $offset Position in list')
            ->newline()
            ->appendParam('return', $typeName);

        $buffer->newline()
            ->append($comment)
            ->append(
                'public function get_' . $field->getCamelCaseName() . 'At($offset)'
            )
            ->append('{')
            ->append(
                'return $this->get(self::' .
                $field->getConstName() . ', $offset);',
                false,
                1
            )
            ->append('}');

        if (!$field->isScalarType())
        {
            $comment = new CommentStringBuffer(self::TAB, self::EOL);
            $comment->append(
                'Returns element from \'' . $field->getName() .
                '\' list at given offset'
            )
                ->newline()
                ->appendParam('param', 'int $offset Position in list')
                ->newline()
                ->appendParam('return', $typeName);

            $buffer->newline()
                ->append($comment)
                ->append(
                    'public function get_' . $field->getCamelCaseName() . 'ObjectAt($offset)'
                )
                ->append('{')
                ->append(
                    '$value = ' .
                    '$this->get(self::' . $field->getConstName() . ', $offset);',
                    false,
                    1
                )
                ->append(
                    '$instance = new ' . $field->getTypeName() . '();',
                    false,
                    1
                )
                ->append('$instance->copy($value);',
                    false,
                    1
                )
                ->append('return $instance;',
                    false,
                    1
                )
                ->append('}');
        }
        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Returns count of \'' . $field->getName() . '\' list')
            ->newline()
            ->appendParam('return', 'int');

        $buffer->newline()
            ->append($comment)
            ->append('public function get_' . $field->getCamelCaseName() . 'Count()')
            ->append('{')
            ->append(
                'return $this->count(self::' . $field->getConstName() . ');',
                false,
                1
            )
            ->append('}');
    }

    /**
     * Describes non-repeated field descriptor
     *
     * @param FieldDescriptor  $field  Field descriptor
     * @param CodeStringBuffer $buffer Buffer to write code to
     *
     * @return null
     */
    private function _describeSingleField(
        FieldDescriptor $field, CodeStringBuffer $buffer
    ) {
        if ($field->isProtobufScalarType() || $field->getTypeDescriptor() instanceof EnumDescriptor) {
            $typeName = $field->getTypeName();
            $argumentClass = '';
        } else {
            $typeName = $this->_createClassName($field->getTypeDescriptor());
            $argumentClass = $typeName . ' ';
        }

        $comment = new CommentStringBuffer(self::TAB, self::EOL);

        $comment->append(
            'Sets value of \'' . $field->getName() . '\' property'
        )
            ->newline()
            ->appendParam(
                'param',
                $typeName . ' $value Property value'
            )
            ->newline()
            ->appendParam('return', 'null');

        $buffer->newline()
            ->append($comment)
            ->append(
                'public function set_' . $field->getCamelCaseName() .
                '(' . $argumentClass . '$value)'
            )
            ->append('{')
            ->append(
                'return $this->set(self::' .
                $field->getConstName() . ', $value);',
                false,
                1
            )
            ->append('}');

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Returns value of \'' . $field->getName() . '\' property')
            ->newline()
            ->appendParam('return', $typeName);

        $buffer->newline()
            ->append($comment)
            ->append('public function get_' . $field->getCamelCaseName() . '()')
            ->append('{')
            ->append(
                'return ' .
                '$this->get(self::' . $field->getConstName() . ');',
                false,
                1
            )
            ->append('}');

        if (!$field->isScalarType())
        {
            $comment = new CommentStringBuffer(self::TAB, self::EOL);
            $comment->append('Returns value of \'' . $field->getName() . '\' property')
                ->newline()
                ->appendParam('return', $typeName);

            $buffer->newline()
                ->append($comment)
                ->append('public function get_' . $field->getCamelCaseName() . 'Object()')
                ->append('{')
                ->append(
                    '$value = ' .
                    '$this->get(self::' . $field->getConstName() . ');',
                    false,
                    1
                )
                ->append(
                    '$instance = new ' . $field->getTypeName() . '();',
                    false,
                    1
                )
                ->append('$instance->copy($value);',
                    false,
                    1
                )
                ->append('return $instance;',
                    false,
                    1
                )
                ->append('}');
        }
    }
    
    /**
     * Generates enum class definition
     *
     * @param array            $enums  Enums descriptors list
     * @param CodeStringBuffer $buffer Buffer to write code
     *
     * @return null
     */
    private function _createEnumClassDefinition(
        array $enums, CodeStringBuffer $buffer
    ) {
        foreach ($enums as $enum) {
            $buffer->append(
                'const ' . $enum->getName() . ' = ' . $enum->getValue() . ';'
            );
        }

        $buffer->newline();

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Returns defined enum values')
            ->newline()
            ->appendParam('return', 'int[]');

        $buffer->append($comment)
            ->append('public function getEnumValues()')
            ->append('{');

        if ($this->_hasSplTypes) {
            $buffer->increaseIdentation()
                ->append('return $this->getConstList(false);');
        } else {
            $buffer->append('return array(', false, 1)
                ->increaseIdentation()
                ->increaseIdentation();

            foreach ($enums as $enum) {
                $buffer->append('\'' . $enum->getName() . '\' => self::' . $enum->getName() . ',');
            }

            $buffer->decreaseIdentation()
                ->append(');');

        }

        $buffer->decreaseIdentation()
            ->append('}');
    }

    /**
     * Generates class constructor and params list
     *
     * @param FieldDescriptor[] $fields Field descriptors list
     * @param CodeStringBuffer  $buffer Code buffer to write to
     *
     * @return null
     */
    private function _createClassConstructor($name, $fields, CodeStringBuffer $buffer)
    {
        $buffer->append('/* Field index constants */');

        foreach ($fields as $field) {
            $buffer->append(
                'const ' . $field->getConstName() .
                ' = ' . $field->getNumber() . ';'
            );
        }

        $buffer->newline();

        $buffer->append('/* @var string class type */')
            ->append('protected static $class_type = "' . $name . '";');
        $buffer->newline();

        $buffer->append('/* @var array Field descriptors */')
            ->append('protected static $fields = array(')
            ->increaseIdentation();

        foreach ($fields as $field) {
            $type = $this->_getType($field);

            $buffer->append('self::' . $field->getConstName() . ' => array(')
                ->increaseIdentation();

            if (!is_null($field->getDefault())) {
                if ($type == ProtobufMessage::PB_TYPE_STRING) {
                    $buffer->append(
                        '\'default\' => \'' .
                        addslashes($field->getDefault()) . '\', '
                    );
                } else {
                    if ($field->isProtobufScalarType()) {
                        $buffer->append(
                            '\'default\' => ' . $field->getDefault() . ', '
                        );
                    } else {
                        $className = $this->_createClassName($field->getTypeDescriptor());
                        $buffer->append(
                            '\'default\' => ' . $className . '::' . $field->getDefault() . ', '
                        );
                    }
                }
            }

            $buffer->append(
                '\'name\' => \'' . addslashes($field->getName()) . '\'' . ','
            );

            if (!$field->isRepeated()) {
                $buffer->append(
                    '\'required\' => ' .
                    ($field->isOptional() ? 'false' : 'true') . ','
                );
            } else {
                $buffer->append('\'repeated\' => true,');
            }

            if (is_int($type)) {
                $buffer->append('\'type\' => ' . $type . ',');
            } else {
                $buffer->append('\'type\' => \'' . $type . '\'');
            }

            $buffer->decreaseIdentation();
            $buffer->append('),');
        }

        $buffer->decreaseIdentation()
            ->append(');')
            ->newline();

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append(
            'Constructs new message container and clears its internal state'
        )
            ->newline()
            ->appendParam('return', 'null');

        $buffer->append($comment)
            ->append('public function __construct()')
            ->append('{')
            ->increaseIdentation()
            ->append('$this->reset();')
            ->decreaseIdentation()
            ->append('}')
            ->newline();

        $buffer->append($comment)
            ->append('public function copy($obj)')
            ->append('{')
            ->increaseIdentation()
            ->append('$this->values = $obj->values;')
            ->decreaseIdentation()
            ->append('}')
            ->newline();

        $comment = new CommentStringBuffer(self::TAB, self::EOL);
        $comment->append('Clears message values and sets default ones')
            ->newline()
            ->appendParam('return', 'null');

        $buffer->append($comment)
            ->append('public function reset()')
            ->append('{')
            ->increaseIdentation();

        foreach ($fields as $field) {
            $type = $this->_getType($field);

            if ($field->isRepeated()) {
                $buffer->append(
                    '$this->values[self::' . $field->getConstName() . '] = array();'
                );
            } else if ($field->isOptional() && !is_null($field->getDefault())) {
                if ($field->isProtobufScalarType()) {
                    $buffer->append(
                        '$this->values[self::' . $field->getConstName() . '] = ' .
                        $field->getDefault() . ';'
                    );
                } else {
                    $className = $this->_createClassName($field->getTypeDescriptor());
                    $buffer->append(
                        '$this->values[self::' . $field->getConstName() . '] = ' .
                        $className . '::' . $field->getDefault() . ';'
                    );
                }
            } else {
                $buffer->append(
                    '$this->values[self::' . $field->getConstName() . '] = null;'
                );
            }
        }

        $buffer->decreaseIdentation()
            ->append('}')
            ->newline();
    }

    /**
     * Parses protobuf file and returns MessageDescriptor
     *
     * @param FileDescriptor    $file           File descriptors
     * @param string            $messageContent Protobuf message content
     * @param MessageDescriptor $parent         Parent message (if nested)
     *
     * @return MessageDescriptor
     *
     * @throws Exception
     */
    private function _parseMessageType(
        FileDescriptor $file, $messageContent, MessageDescriptor $parent = null,
        ServiceDescriptor $service = null
    ) {
        if ($messageContent == '') {
            return;
        }

        while (strlen($messageContent) > 0) {
            $next = ($this->_next($messageContent));

            if (strtolower($next) == 'message') {
                $messageContent = trim(substr($messageContent, strlen($next)));
                $name = $this->_next($messageContent);

                $offset = $this->_getBeginEnd($messageContent, '{', '}');
                // now extract the content and call parse_message again
                $content = trim(
                    substr(
                        $messageContent,
                        $offset['begin'] + 1,
                        $offset['end'] - $offset['begin'] - 2
                    )
                );

                $childMessage = new MessageDescriptor($name, $file, $parent);
                $this->_parseMessageType($file, $content, $childMessage, $service);

                $messageContent = '' . trim(substr($messageContent, $offset['end']));
            } else if (strtolower($next) == 'service') {
                $messageContent = trim(substr($messageContent, strlen($next)));
                $name = $this->_next($messageContent);

                $offset = $this->_getBeginEnd($messageContent, '{', '}');
                // now extract the content and call parse_message again
                $content = trim(
                    substr(
                        $messageContent,
                        $offset['begin'] + 1,
                        $offset['end'] - $offset['begin'] - 2
                    )
                );

                $rpcService = new ServiceDescriptor($name, $file, $service);
                $this->_parseMessageType($file, $content, $parent, $rpcService);

                $messageContent = '' . trim(substr($messageContent, $offset['end']));
            } else if (strtolower($next) == 'rpc') {
                $messageContent = trim(substr($messageContent, strlen($next)));
                $func_input = $this->_next($messageContent);
                $func_output = trim(substr($messageContent, strlen($func_input)));
                $offset_input = $this->_getBeginEnd($func_input, '(', ')');
                $name = substr($func_input, 0, strpos($func_input, '(', 0));               
                $input_type = trim(
                    substr(
                        $func_input,
                        $offset_input['begin'] + 1,
                        $offset_input['end'] - $offset_input['begin'] - 2
                    )
                );
                $offset_output = $this->_getBeginEnd($func_output, '(', ')');
                $output_type = trim(
                    substr(
                        $func_output,
                        $offset_output['begin'] + 1,
                        $offset_output['end'] - $offset_output['begin'] - 2
                    )
                );
                // now extract the content and call parse_message again

                $method = new MethodDescriptor($name, $input_type, $output_type, $file, $service);
                // removing it from string
                $messageContent = substr(
                    $messageContent,
                    strpos($messageContent, ';') + 1,
                    strlen($messageContent)
                );
                $messageContent = '' . trim($messageContent);
            } else if (strtolower($next) == 'enum') {
                $messageContent = trim(substr($messageContent, strlen($next)));
                $name = $this->_next($messageContent);
                $offset = $this->_getBeginEnd($messageContent, '{', '}');
                // now extract the content and call parse_message again
                $content = trim(
                    substr(
                        $messageContent,
                        $offset['begin'] + 1,
                        $offset['end'] - $offset['begin'] - 2
                    )
                );

                $enum = new EnumDescriptor($name, $file, $parent);
                $this->_parseEnum($enum, $content);
                // removing it from string
                $messageContent = '' . trim(substr($messageContent, $offset['end']));
            } else if (strtolower($next) == 'import') {
                $name = $this->_next($messageContent);

                $match = preg_match(
                    '/"([^"]+)";*\s?/',
                    $messageContent,
                    $matches,
                    PREG_OFFSET_CAPTURE
                );

                if (!$match) {
                    throw new Exception(
                        'Malformed include / look at your import statement: ' .
                        $messageContent
                    );
                }

                $includedFilename = $matches[1][0];

                if (!file_exists($includedFilename)) {
                    throw new Exception(
                        'Included file ' . $includedFilename . ' does not exist'
                    );
                }

                $messageContent = trim(
                    substr(
                        $messageContent,
                        $matches[0][1] + strlen($matches[0][0])
                    )
                );

                $parserKey = realpath($includedFilename);

                if (!isset(self::$_parsers[$parserKey])) {
                    $pbp = new ProtobufParser($this->_useNativeNamespaces);
                    self::$_parsers[$parserKey] = $pbp;
                }

                $file->addDependency($pbp->parse($includedFilename));

            } else if (strtolower($next) == 'option') {

                // We don't support option parameters just yet, skip for now.
                $messageContent = preg_replace('/^.+\n/', '', $messageContent);
                
            } else if (strtolower($next) == 'package') {

                $match = preg_match(
                    '/package[\s]+([^;]+);?/',
                    $messageContent,
                    $matches,
                    PREG_OFFSET_CAPTURE
                );

                if (!$match) {
                    throw new Exception('Malformed package');
                }

                $file->setPackage($matches[1][0]);
                $messageContent = trim(
                    substr(
                        $messageContent,
                        $matches[0][1] + strlen($matches[0][0])
                    )
                );
            } else {
                // now a normal field
                $match = preg_match(
                    '/(.*);/',
                    $messageContent,
                    $matches,
                    PREG_OFFSET_CAPTURE
                );

                if (!$match || !$parent) {
                    throw new Exception('Proto file missformed');
                }

                $parent->addField($this->_parseField($matches[0][0]));

                $messageContent = trim(
                    substr(
                        $messageContent,
                        $matches[0][1] + strlen($matches[0][0])
                    )
                );
            }
        }
    }

    /**
     * Parses protobuf field properties
     *
     * @param string $content Protobuf content
     *
     * @return FieldDescriptor
     * @throws Exception
     */
    private function _parseField($content)
    {
        $field = new FieldDescriptor();

        // parse the default value
        $match = preg_match(
            '/\[\s?default\s?=\s?([^\[]*)\]\s?;/',
            $content,
            $matches,
            PREG_OFFSET_CAPTURE
        );

        if ($match) {
            $field->setDefault($matches[1][0]);
            $content = trim(substr($content, 0, $matches[0][1])) . ';';
        }

        // parse the value
        $match = preg_match(
            '/(optional|required|repeated)[\s]+([^;^=^\s^]+)' .
            '[\s]+([^;^=^\s]+)[\s]+=[\s]+([\d]+);/',
            $content,
            $matches
        );

        if ($match) {
            switch ($matches[1]) {

            case 'optional':
                $label = FieldLabel::OPTIONAL;
                break;

            case 'required':
                $label = FieldLabel::REQUIRED;
                break;

            case 'repeated':
                $label = FieldLabel::REPEATED;
                break;
            }

            $field->setLabel($label);

            if (($pos = strrpos($matches[2], '.')) !== false) {
                $field->setType(substr($matches[2], $pos + 1));

                if ($pos == 0) {
                    $field->setNamespace($matches[2][$pos]);
                } else {
                    $field->setNamespace(substr($matches[2], 0, $pos));
                }
            } else {
                $field->setType($matches[2]);
            }

            $field->setName($matches[3]);
            $field->setNumber($matches[4]);
        } else {
            throw new Exception('Syntax error ' . $content);
        }

        return $field;
    }

    /**
     * Resolves message field types
     *
     * @param MessageDescriptor $descriptor Message descriptor
     * @param FileDescriptor    $file       File descriptor
     *
     * @throws Exception
     * @return null
     */
    private function _resolveMessageFieldTypes(
        MessageDescriptor $descriptor, FileDescriptor $file
    ) {
        foreach ($descriptor->getFields() as $field) {

            if ($field->isProtobufScalarType()) {
                continue;
            }

            $namespace = $field->getNamespace();

            if (is_null($namespace)) {
                if (($type = $descriptor->findType($field->getType())) !== false) {
                    $field->setTypeDescriptor($type);
                    continue;
                }

                $exists = $this->_namespaces[$file->getPackage()]
                    [$field->getType()];

                if (isset($exists)) {

                    $field->setTypeDescriptor(
                        $this->_namespaces[$file->getPackage()][$field->getType()]
                    );

                    continue;
                }

                $exists = isset($this->_namespaces[self::$_globalNamespace])
                    && isset($this->_namespaces[self::$_globalNamespace]
                    [$field->getType()]);

                if ($exists) {

                    $field->setTypeDescriptor(
                        $this->_namespaces[self::GLOBAL_NAMESPACE][$field->getType()]
                    );

                    continue;
                }

                throw new Exception('Type ' . $field->getType() . ' not defined');
            } else if ($namespace[0] == '.') {
                if ($namespace == '.') {
                    $namespace = '';
                } else {
                    $namespace = substr($namespace, 1);
                }

                if (!isset($this->_namespaces[$namespace])) {
                    throw new Exception(
                        'Namespace \'' . $namespace . '\' for type ' .
                        $field->getType() . ' not defined'
                    );
                }

                if (!isset($this->_namespaces[$namespace][$field->getType()])) {
                    throw new Exception(
                        'Type ' . $field->getType() . ' not defined in ' . $namespace
                    );
                }

                $field->setTypeDescriptor(
                    $this->_namespaces[$namespace][$field->getType()]
                );

            } else {
                $type = $descriptor->findType(
                    $field->getType(), $field->getNamespace()
                );

                if ($type !== false) {
                    $field->setTypeDescriptor($type);
                    continue;
                }

                if (!isset($this->_namespaces[$namespace])) {
                    throw new Exception(
                        'Namespace ' . $namespace . ' for type ' .
                        $field->getType() . ' not defined'
                    );
                }

                $exists = isset($this->_namespaces[$namespace][$field->getType()]);

                if (!$exists) {
                    throw new Exception(
                        'Type ' . $field->getType() . ' not defined in ' . $namespace
                    );
                }

                $field->setTypeDescriptor(
                    $this->_namespaces[$namespace][$field->getType()]
                );
            }
        }

        foreach ($descriptor->getNested() as $nested) {
            $this->_resolveMessageFieldTypes($nested, $file);
        }
    }

    /**
     * Resolves namespaces for given file descriptor
     *
     * @param FileDescriptor $file File descriptor
     *
     * @return null
     */
    private function _resolveNamespaces(FileDescriptor $file)
    {
        foreach ($file->getEnums() as $descriptor) {
            $this->_namespaces[$file->getPackage()]
            [$descriptor->getName()] = $descriptor;
        }

        foreach ($file->getMessages() as $name => $descriptor) {
            $this->_namespaces[$file->getPackage()]
            [$descriptor->getName()] = $descriptor;
        }

        foreach (self::$_parsers as $parser) {
            foreach ($parser->getNamespaces() as $namespace => $descriptors) {
                foreach ($descriptors as $name => $descriptor) {
                    $this->_namespaces[$namespace][$name] = $descriptor;
                }
            }
        }

        foreach ($file->getMessages() as $name => $descriptor) {
            $this->_resolveMessageFieldTypes($descriptor, $file);
        }
    }

    /**
     * Parses enum
     *
     * @param EnumDescriptor $enum    Enum descriptor
     * @param string         $content Protobuf enum description
     *
     * @return EnumDescriptor
     * @throws Exception
     */
    private function _parseEnum(EnumDescriptor $enum, $content)
    {
        $match = preg_match_all('/(.*);\s?/', $content, $matches);

        if (!$match) {
            throw new \Exception('Semantic error in Enum!');
        }

        foreach ($matches[1] as $match) {
            $split = preg_split('/=/', $match);

            $enum->addValue(
                new EnumValueDescriptor(trim($split[0]), trim($split[1]))
            );
        }

        return $enum;
    }

    /**
     * Returns next token from parsed protobuf message
     *
     * @param string $message Message to parse
     *
     * @return int|string Next token or -1 if not found
     */
    private function _next($message)
    {
        $match = preg_match(
            '/([^\s^\{}]*)/',
            $message,
            $matches,
            PREG_OFFSET_CAPTURE
        );

        if (!$match) {
            return -1;
        } else {
            return trim($matches[0][0]);
        }
    }


    /**
     * Finds starting, ending char in string
     *
     * @param string $string  String to search
     * @param string $char    Starting char
     * @param string $charend Ending char
     *
     * @return array
     * @throws Exception
     */
    private function _getBeginEnd($string, $char, $charend)
    {
        $offsetBegin = strpos($string, $char);

        if ($offsetBegin === false) {
            return array('begin' => -1, 'end' => -1);
        }

        $offsetNumber = 1;
        $offset = $offsetBegin + 1;

        while ($offsetNumber > 0 && $offset > 0) {
            // now search after the end nested { }
            $offsetOpen = strpos($string, $char, $offset);
            $offsetClose = strpos($string, $charend, $offset);

            if ($offsetOpen < $offsetClose && !($offsetOpen === false)) {
                $offset = $offsetOpen + 1;
                $offsetNumber++;
            } else if (!($offsetClose === false)) {
                $offset = $offsetClose + 1;
                $offsetNumber--;
            } else {
                $offset = -1;
            }
        }

        if ($offset == -1) {
            throw new Exception('Protofile failure: ' . $char . ' not nested');
        }

        return array('begin' => $offsetBegin, 'end' => $offset);
    }

    /**
     * Strips comments in string
     *
     * @param string &$string String to be stripped
     *
     * @return null
     */
    private function _stripComments(&$string)
    {
        $string = preg_replace('/\/\/.*/', '', $string);
        // now replace empty lines and whitespaces in front
        $string = preg_replace('/\\r?\\n\s*/', PHP_EOL, $string);
    }
}

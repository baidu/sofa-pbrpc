<?php
/**
 * Helper class for generating source code
 */
class CodeStringBuffer
{
    private $_buffer = array();
    private $_identLevel = 0;

    protected $identStr = '';
    protected $newLineStr = '';

    /**
     * Constructs new code buffer
     *
     * @param string $tabString     String used for identation
     * @param string $newLineString String used as new line
     */
    public function __construct($tabString = ' ', $newLineString = PHP_EOL)
    {
        $this->identStr = $tabString;
        $this->newLineStr = $newLineString;
    }

    /**
     * Adds new line to buffer
     *
     * @return CodeStringBuffer
     */
    public function newline()
    {
        $this->append('');
        return $this;
    }

    /**
     * Appends lines to buffer
     *
     * @param string $lines       Lines to append
     * @param bool   $newline     Add extra newline after lines
     * @param int    $identOffset Extra ident code
     *
     * @return CodeStringBuffer
     */
    public function append($lines, $newline = false, $identOffset = 0)
    {
        foreach (explode($this->newLineStr, $lines) as $line) {
            if (strlen($line) > 0) {
                $this->_buffer[] = $this->_getIdentationString($identOffset) . $line;
            } else {
                $this->_buffer[] = '';
            }
        }

        if ($newline) {
            $this->_buffer[] = $this->newLineStr;
        }

        return $this;
    }

    /**
     * Increases identation
     *
     * @return CodeStringBuffer
     */
    public function increaseIdentation()
    {
        $this->_identLevel++;

        return $this;
    }

    /**
     * Decreases identation
     *
     * @return CodeStringBuffer
     */
    public function decreaseIdentation()
    {
        $this->_identLevel = max(0, $this->_identLevel - 1);

        return $this;
    }

    /**
     * Returns identation string
     *
     * @param int $identOffset Offset
     *
     * @return string
     */
    private function _getIdentationString($identOffset = 0)
    {
        return str_repeat($this->identStr, $this->_identLevel + $identOffset);
    }

    /**
     * Returns buffer as string
     *
     * @return string
     */
    public function __toString()
    {
        return implode($this->newLineStr, $this->_buffer);
    }
}


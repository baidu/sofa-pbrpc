<?php
/**
 * Helper class for generating source code comments
 */
class CommentStringBuffer extends CodeStringBuffer
{
    const COMMENT_LINE_PREFIX = ' *';

    /**
     * Appends new param to docblock
     *
     * @param string $param Param name
     * @param string $value Param value
     *
     * @return CommentStringBuffer
     */
    public function appendParam($param, $value)
    {
        $this->append('@' . $param . ' ' . $value);

        return $this;
    }

    /**
     * Appends new line to docblock
     *
     * @return CommentStringBuffer
     */
    public function newline()
    {
        $this->append('');

        return $this;
    }

    /**
     * Appends new comment line to block
     *
     * @param string $line        Lines to append
     * @param bool   $newline     True to append extra line at the end
     * @param int    $identOffset Ident offset
     *
     * @return CommentStringBuffer
     */
    public function append($line, $newline = false, $identOffset = 0)
    {
        if (strlen($line) > 0) {
            parent::append(
                self::COMMENT_LINE_PREFIX . ' ' . $line, $newline, $identOffset
            );
        } else {
            parent::append(
                self::COMMENT_LINE_PREFIX, $newline, $identOffset
            );
        }

        return $this;
    }

    /**
     * Returns buffer as as string
     *
     * @return string
     */
    public function __toString()
    {
        return '/**' . $this->newLineStr .
            parent::__toString() .
            $this->newLineStr . ' */';
    }
}

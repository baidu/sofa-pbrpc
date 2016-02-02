<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * XML_Util
 *
 * XML Utilities package
 *
 * PHP versions 4 and 5
 *
 * LICENSE:
 *
 * Copyright (c) 2003-2008 Stephan Schmidt <schst@php.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @category  XML
 * @package   XML_Util
 * @author    Stephan Schmidt <schst@php.net>
 * @copyright 2003-2008 Stephan Schmidt <schst@php.net>
 * @license   http://opensource.org/licenses/bsd-license New BSD License
 * @version   CVS: $Id$
 * @link      http://pear.php.net/package/XML_Util
 */

/**
 * error code for invalid chars in XML name
 */
define('XML_UTIL_ERROR_INVALID_CHARS', 51);

/**
 * error code for invalid chars in XML name
 */
define('XML_UTIL_ERROR_INVALID_START', 52);

/**
 * error code for non-scalar tag content
 */
define('XML_UTIL_ERROR_NON_SCALAR_CONTENT', 60);

/**
 * error code for missing tag name
 */
define('XML_UTIL_ERROR_NO_TAG_NAME', 61);

/**
 * replace XML entities
 */
define('XML_UTIL_REPLACE_ENTITIES', 1);

/**
 * embedd content in a CData Section
 */
define('XML_UTIL_CDATA_SECTION', 5);

/**
 * do not replace entitites
 */
define('XML_UTIL_ENTITIES_NONE', 0);

/**
 * replace all XML entitites
 * This setting will replace <, >, ", ' and &
 */
define('XML_UTIL_ENTITIES_XML', 1);

/**
 * replace only required XML entitites
 * This setting will replace <, " and &
 */
define('XML_UTIL_ENTITIES_XML_REQUIRED', 2);

/**
 * replace HTML entitites
 * @link http://www.php.net/htmlentities
 */
define('XML_UTIL_ENTITIES_HTML', 3);

/**
 * Collapse all empty tags.
 */
define('XML_UTIL_COLLAPSE_ALL', 1);

/**
 * Collapse only empty XHTML tags that have no end tag.
 */
define('XML_UTIL_COLLAPSE_XHTML_ONLY', 2);

/**
 * utility class for working with XML documents
 *

 * @category  XML
 * @package   XML_Util
 * @author    Stephan Schmidt <schst@php.net>
 * @copyright 2003-2008 Stephan Schmidt <schst@php.net>
 * @license   http://opensource.org/licenses/bsd-license New BSD License
 * @version   Release: 1.2.3
 * @link      http://pear.php.net/package/XML_Util
 */
class XML_Util
{
    /**
     * return API version
     *
     * @return string $version API version
     * @access public
     * @static
     */
    function apiVersion()
    {
        return '1.1';
    }

    /**
     * replace XML entities
     *
     * With the optional second parameter, you may select, which
     * entities should be replaced.
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // replace XML entites:
     * $string = XML_Util::replaceEntities('This string contains < & >.');
     * </code>
     *
     * With the optional third parameter, you may pass the character encoding
     * <code>
     * require_once 'XML/Util.php';
     *
     * // replace XML entites in UTF-8:
     * $string = XML_Util::replaceEntities(
     *     'This string contains < & > as well as ä, ö, ß, à and ê',
     *     XML_UTIL_ENTITIES_HTML,
     *     'UTF-8'
     * );
     * </code>
     *
     * @param string $string          string where XML special chars
     *                                should be replaced
     * @param int    $replaceEntities setting for entities in attribute values
     *                                (one of XML_UTIL_ENTITIES_XML,
     *                                XML_UTIL_ENTITIES_XML_REQUIRED,
     *                                XML_UTIL_ENTITIES_HTML)
     * @param string $encoding        encoding value (if any)...
     *                                must be a valid encoding as determined
     *                                by the htmlentities() function
     *
     * @return string string with replaced chars
     * @access public
     * @static
     * @see reverseEntities()
     */
    function replaceEntities($string, $replaceEntities = XML_UTIL_ENTITIES_XML,
        $encoding = 'ISO-8859-1')
    {
        switch ($replaceEntities) {
        case XML_UTIL_ENTITIES_XML:
            return strtr($string, array(
                '&'  => '&amp;',
                '>'  => '&gt;',
                '<'  => '&lt;',
                '"'  => '&quot;',
                '\'' => '&apos;' ));
            break;
        case XML_UTIL_ENTITIES_XML_REQUIRED:
            return strtr($string, array(
                '&' => '&amp;',
                '<' => '&lt;',
                '"' => '&quot;' ));
            break;
        case XML_UTIL_ENTITIES_HTML:
            return htmlentities($string, ENT_COMPAT, $encoding);
            break;
        }
        return $string;
    }

    /**
     * reverse XML entities
     *
     * With the optional second parameter, you may select, which
     * entities should be reversed.
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // reverse XML entites:
     * $string = XML_Util::reverseEntities('This string contains &lt; &amp; &gt;.');
     * </code>
     *
     * With the optional third parameter, you may pass the character encoding
     * <code>
     * require_once 'XML/Util.php';
     *
     * // reverse XML entites in UTF-8:
     * $string = XML_Util::reverseEntities(
     *     'This string contains &lt; &amp; &gt; as well as'
     *     . ' &auml;, &ouml;, &szlig;, &agrave; and &ecirc;',
     *     XML_UTIL_ENTITIES_HTML,
     *     'UTF-8'
     * );
     * </code>
     *
     * @param string $string          string where XML special chars
     *                                should be replaced
     * @param int    $replaceEntities setting for entities in attribute values
     *                                (one of XML_UTIL_ENTITIES_XML,
     *                                XML_UTIL_ENTITIES_XML_REQUIRED,
     *                                XML_UTIL_ENTITIES_HTML)
     * @param string $encoding        encoding value (if any)...
     *                                must be a valid encoding as determined
     *                                by the html_entity_decode() function
     *
     * @return string string with replaced chars
     * @access public
     * @static
     * @see replaceEntities()
     */
    function reverseEntities($string, $replaceEntities = XML_UTIL_ENTITIES_XML,
        $encoding = 'ISO-8859-1')
    {
        switch ($replaceEntities) {
        case XML_UTIL_ENTITIES_XML:
            return strtr($string, array(
                '&amp;'  => '&',
                '&gt;'   => '>',
                '&lt;'   => '<',
                '&quot;' => '"',
                '&apos;' => '\'' ));
            break;
        case XML_UTIL_ENTITIES_XML_REQUIRED:
            return strtr($string, array(
                '&amp;'  => '&',
                '&lt;'   => '<',
                '&quot;' => '"' ));
            break;
        case XML_UTIL_ENTITIES_HTML:
            return html_entity_decode($string, ENT_COMPAT, $encoding);
            break;
        }
        return $string;
    }

    /**
     * build an xml declaration
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // get an XML declaration:
     * $xmlDecl = XML_Util::getXMLDeclaration('1.0', 'UTF-8', true);
     * </code>
     *
     * @param string $version    xml version
     * @param string $encoding   character encoding
     * @param bool   $standalone document is standalone (or not)
     *
     * @return string xml declaration
     * @access public
     * @static
     * @uses attributesToString() to serialize the attributes of the XML declaration
     */
    function getXMLDeclaration($version = '1.0', $encoding = null,
        $standalone = null)
    {
        $attributes = array(
            'version' => $version,
        );
        // add encoding
        if ($encoding !== null) {
            $attributes['encoding'] = $encoding;
        }
        // add standalone, if specified
        if ($standalone !== null) {
            $attributes['standalone'] = $standalone ? 'yes' : 'no';
        }

        return sprintf('<?xml%s?>',
            XML_Util::attributesToString($attributes, false));
    }

    /**
     * build a document type declaration
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // get a doctype declaration:
     * $xmlDecl = XML_Util::getDocTypeDeclaration('rootTag','myDocType.dtd');
     * </code>
     *
     * @param string $root        name of the root tag
     * @param string $uri         uri of the doctype definition
     *                            (or array with uri and public id)
     * @param string $internalDtd internal dtd entries
     *
     * @return string doctype declaration
     * @access public
     * @static
     * @since 0.2
     */
    function getDocTypeDeclaration($root, $uri = null, $internalDtd = null)
    {
        if (is_array($uri)) {
            $ref = sprintf(' PUBLIC "%s" "%s"', $uri['id'], $uri['uri']);
        } elseif (!empty($uri)) {
            $ref = sprintf(' SYSTEM "%s"', $uri);
        } else {
            $ref = '';
        }

        if (empty($internalDtd)) {
            return sprintf('<!DOCTYPE %s%s>', $root, $ref);
        } else {
            return sprintf("<!DOCTYPE %s%s [\n%s\n]>", $root, $ref, $internalDtd);
        }
    }

    /**
     * create string representation of an attribute list
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // build an attribute string
     * $att = array(
     *              'foo'   =>  'bar',
     *              'argh'  =>  'tomato'
     *            );
     *
     * $attList = XML_Util::attributesToString($att);
     * </code>
     *
     * @param array      $attributes attribute array
     * @param bool|array $sort       sort attribute list alphabetically,
     *                               may also be an assoc array containing
     *                               the keys 'sort', 'multiline', 'indent',
     *                               'linebreak' and 'entities'
     * @param bool       $multiline  use linebreaks, if more than
     *                               one attribute is given
     * @param string     $indent     string used for indentation of
     *                               multiline attributes
     * @param string     $linebreak  string used for linebreaks of
     *                               multiline attributes
     * @param int        $entities   setting for entities in attribute values
     *                               (one of XML_UTIL_ENTITIES_NONE,
     *                               XML_UTIL_ENTITIES_XML,
     *                               XML_UTIL_ENTITIES_XML_REQUIRED,
     *                               XML_UTIL_ENTITIES_HTML)
     *
     * @return string string representation of the attributes
     * @access public
     * @static
     * @uses replaceEntities() to replace XML entities in attribute values
     * @todo allow sort also to be an options array
     */
    function attributesToString($attributes, $sort = true, $multiline = false,
        $indent = '    ', $linebreak = "\n", $entities = XML_UTIL_ENTITIES_XML)
    {
        /*
         * second parameter may be an array
         */
        if (is_array($sort)) {
            if (isset($sort['multiline'])) {
                $multiline = $sort['multiline'];
            }
            if (isset($sort['indent'])) {
                $indent = $sort['indent'];
            }
            if (isset($sort['linebreak'])) {
                $multiline = $sort['linebreak'];
            }
            if (isset($sort['entities'])) {
                $entities = $sort['entities'];
            }
            if (isset($sort['sort'])) {
                $sort = $sort['sort'];
            } else {
                $sort = true;
            }
        }
        $string = '';
        if (is_array($attributes) && !empty($attributes)) {
            if ($sort) {
                ksort($attributes);
            }
            if ( !$multiline || count($attributes) == 1) {
                foreach ($attributes as $key => $value) {
                    if ($entities != XML_UTIL_ENTITIES_NONE) {
                        if ($entities === XML_UTIL_CDATA_SECTION) {
                            $entities = XML_UTIL_ENTITIES_XML;
                        }
                        $value = XML_Util::replaceEntities($value, $entities);
                    }
                    $string .= ' ' . $key . '="' . $value . '"';
                }
            } else {
                $first = true;
                foreach ($attributes as $key => $value) {
                    if ($entities != XML_UTIL_ENTITIES_NONE) {
                        $value = XML_Util::replaceEntities($value, $entities);
                    }
                    if ($first) {
                        $string .= ' ' . $key . '="' . $value . '"';
                        $first   = false;
                    } else {
                        $string .= $linebreak . $indent . $key . '="' . $value . '"';
                    }
                }
            }
        }
        return $string;
    }

    /**
     * Collapses empty tags.
     *
     * @param string $xml  XML
     * @param int    $mode Whether to collapse all empty tags (XML_UTIL_COLLAPSE_ALL)
     *                      or only XHTML (XML_UTIL_COLLAPSE_XHTML_ONLY) ones.
     *
     * @return string XML
     * @access public
     * @static
     * @todo PEAR CS - unable to avoid "space after open parens" error
     *       in the IF branch
     */
    function collapseEmptyTags($xml, $mode = XML_UTIL_COLLAPSE_ALL)
    {
        if ($mode == XML_UTIL_COLLAPSE_XHTML_ONLY) {
            return preg_replace(
                '/<(area|base(?:font)?|br|col|frame|hr|img|input|isindex|link|meta|'
                . 'param)([^>]*)><\/\\1>/s',
                '<\\1\\2 />',
                $xml);
        } else {
            return preg_replace('/<(\w+)([^>]*)><\/\\1>/s', '<\\1\\2 />', $xml);
        }
    }

    /**
     * create a tag
     *
     * This method will call XML_Util::createTagFromArray(), which
     * is more flexible.
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // create an XML tag:
     * $tag = XML_Util::createTag('myNs:myTag',
     *     array('foo' => 'bar'),
     *     'This is inside the tag',
     *     'http://www.w3c.org/myNs#');
     * </code>
     *
     * @param string $qname           qualified tagname (including namespace)
     * @param array  $attributes      array containg attributes
     * @param mixed  $content         the content
     * @param string $namespaceUri    URI of the namespace
     * @param int    $replaceEntities whether to replace XML special chars in
     *                                content, embedd it in a CData section
     *                                or none of both
     * @param bool   $multiline       whether to create a multiline tag where
     *                                each attribute gets written to a single line
     * @param string $indent          string used to indent attributes
     *                                (_auto indents attributes so they start
     *                                at the same column)
     * @param string $linebreak       string used for linebreaks
     * @param bool   $sortAttributes  Whether to sort the attributes or not
     *
     * @return string XML tag
     * @access public
     * @static
     * @see createTagFromArray()
     * @uses createTagFromArray() to create the tag
     */
    function createTag($qname, $attributes = array(), $content = null,
        $namespaceUri = null, $replaceEntities = XML_UTIL_REPLACE_ENTITIES,
        $multiline = false, $indent = '_auto', $linebreak = "\n",
        $sortAttributes = true)
    {
        $tag = array(
            'qname'      => $qname,
            'attributes' => $attributes
        );

        // add tag content
        if ($content !== null) {
            $tag['content'] = $content;
        }

        // add namespace Uri
        if ($namespaceUri !== null) {
            $tag['namespaceUri'] = $namespaceUri;
        }

        return XML_Util::createTagFromArray($tag, $replaceEntities, $multiline,
            $indent, $linebreak, $sortAttributes);
    }

    /**
     * create a tag from an array
     * this method awaits an array in the following format
     * <pre>
     * array(
     *     // qualified name of the tag
     *     'qname' => $qname
     *
     *     // namespace prefix (optional, if qname is specified or no namespace)
     *     'namespace' => $namespace
     *
     *     // local part of the tagname (optional, if qname is specified)
     *     'localpart' => $localpart,
     *
     *     // array containing all attributes (optional)
     *     'attributes' => array(),
     *
     *     // tag content (optional)
     *     'content' => $content,
     *
     *     // namespaceUri for the given namespace (optional)
     *     'namespaceUri' => $namespaceUri
     * )
     * </pre>
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * $tag = array(
     *     'qname'        => 'foo:bar',
     *     'namespaceUri' => 'http://foo.com',
     *     'attributes'   => array('key' => 'value', 'argh' => 'fruit&vegetable'),
     *     'content'      => 'I\'m inside the tag',
     * );
     * // creating a tag with qualified name and namespaceUri
     * $string = XML_Util::createTagFromArray($tag);
     * </code>
     *
     * @param array  $tag             tag definition
     * @param int    $replaceEntities whether to replace XML special chars in
     *                                content, embedd it in a CData section
     *                                or none of both
     * @param bool   $multiline       whether to create a multiline tag where each
     *                                attribute gets written to a single line
     * @param string $indent          string used to indent attributes
     *                                (_auto indents attributes so they start
     *                                at the same column)
     * @param string $linebreak       string used for linebreaks
     * @param bool   $sortAttributes  Whether to sort the attributes or not
     *
     * @return string XML tag
     * @access public
     * @static
     * @see createTag()
     * @uses attributesToString() to serialize the attributes of the tag
     * @uses splitQualifiedName() to get local part and namespace of a qualified name
     * @uses createCDataSection()
     * @uses raiseError()
     */
    function createTagFromArray($tag, $replaceEntities = XML_UTIL_REPLACE_ENTITIES,
        $multiline = false, $indent = '_auto', $linebreak = "\n",
        $sortAttributes = true)
    {
        if (isset($tag['content']) && !is_scalar($tag['content'])) {
            return XML_Util::raiseError('Supplied non-scalar value as tag content',
            XML_UTIL_ERROR_NON_SCALAR_CONTENT);
        }

        if (!isset($tag['qname']) && !isset($tag['localPart'])) {
            return XML_Util::raiseError('You must either supply a qualified name '
                . '(qname) or local tag name (localPart).',
                XML_UTIL_ERROR_NO_TAG_NAME);
        }

        // if no attributes hav been set, use empty attributes
        if (!isset($tag['attributes']) || !is_array($tag['attributes'])) {
            $tag['attributes'] = array();
        }

        if (isset($tag['namespaces'])) {
            foreach ($tag['namespaces'] as $ns => $uri) {
                $tag['attributes']['xmlns:' . $ns] = $uri;
            }
        }

        if (!isset($tag['qname'])) {
            // qualified name is not given

            // check for namespace
            if (isset($tag['namespace']) && !empty($tag['namespace'])) {
                $tag['qname'] = $tag['namespace'] . ':' . $tag['localPart'];
            } else {
                $tag['qname'] = $tag['localPart'];
            }
        } elseif (isset($tag['namespaceUri']) && !isset($tag['namespace'])) {
            // namespace URI is set, but no namespace

            $parts = XML_Util::splitQualifiedName($tag['qname']);

            $tag['localPart'] = $parts['localPart'];
            if (isset($parts['namespace'])) {
                $tag['namespace'] = $parts['namespace'];
            }
        }

        if (isset($tag['namespaceUri']) && !empty($tag['namespaceUri'])) {
            // is a namespace given
            if (isset($tag['namespace']) && !empty($tag['namespace'])) {
                $tag['attributes']['xmlns:' . $tag['namespace']] =
                    $tag['namespaceUri'];
            } else {
                // define this Uri as the default namespace
                $tag['attributes']['xmlns'] = $tag['namespaceUri'];
            }
        }

        // check for multiline attributes
        if ($multiline === true) {
            if ($indent === '_auto') {
                $indent = str_repeat(' ', (strlen($tag['qname'])+2));
            }
        }

        // create attribute list
        $attList = XML_Util::attributesToString($tag['attributes'],
            $sortAttributes, $multiline, $indent, $linebreak);
        if (!isset($tag['content']) || (string)$tag['content'] == '') {
            $tag = sprintf('<%s%s />', $tag['qname'], $attList);
        } else {
            switch ($replaceEntities) {
            case XML_UTIL_ENTITIES_NONE:
                break;
            case XML_UTIL_CDATA_SECTION:
                $tag['content'] = XML_Util::createCDataSection($tag['content']);
                break;
            default:
                $tag['content'] = XML_Util::replaceEntities($tag['content'],
                    $replaceEntities);
                break;
            }
            $tag = sprintf('<%s%s>%s</%s>', $tag['qname'], $attList, $tag['content'],
                $tag['qname']);
        }
        return $tag;
    }

    /**
     * create a start element
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // create an XML start element:
     * $tag = XML_Util::createStartElement('myNs:myTag',
     *     array('foo' => 'bar') ,'http://www.w3c.org/myNs#');
     * </code>
     *
     * @param string $qname          qualified tagname (including namespace)
     * @param array  $attributes     array containg attributes
     * @param string $namespaceUri   URI of the namespace
     * @param bool   $multiline      whether to create a multiline tag where each
     *                               attribute gets written to a single line
     * @param string $indent         string used to indent attributes (_auto indents
     *                               attributes so they start at the same column)
     * @param string $linebreak      string used for linebreaks
     * @param bool   $sortAttributes Whether to sort the attributes or not
     *
     * @return string XML start element
     * @access public
     * @static
     * @see createEndElement(), createTag()
     */
    function createStartElement($qname, $attributes = array(), $namespaceUri = null,
        $multiline = false, $indent = '_auto', $linebreak = "\n",
        $sortAttributes = true)
    {
        // if no attributes hav been set, use empty attributes
        if (!isset($attributes) || !is_array($attributes)) {
            $attributes = array();
        }

        if ($namespaceUri != null) {
            $parts = XML_Util::splitQualifiedName($qname);
        }

        // check for multiline attributes
        if ($multiline === true) {
            if ($indent === '_auto') {
                $indent = str_repeat(' ', (strlen($qname)+2));
            }
        }

        if ($namespaceUri != null) {
            // is a namespace given
            if (isset($parts['namespace']) && !empty($parts['namespace'])) {
                $attributes['xmlns:' . $parts['namespace']] = $namespaceUri;
            } else {
                // define this Uri as the default namespace
                $attributes['xmlns'] = $namespaceUri;
            }
        }

        // create attribute list
        $attList = XML_Util::attributesToString($attributes, $sortAttributes,
            $multiline, $indent, $linebreak);
        $element = sprintf('<%s%s>', $qname, $attList);
        return  $element;
    }

    /**
     * create an end element
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // create an XML start element:
     * $tag = XML_Util::createEndElement('myNs:myTag');
     * </code>
     *
     * @param string $qname qualified tagname (including namespace)
     *
     * @return string XML end element
     * @access public
     * @static
     * @see createStartElement(), createTag()
     */
    function createEndElement($qname)
    {
        $element = sprintf('</%s>', $qname);
        return $element;
    }

    /**
     * create an XML comment
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // create an XML start element:
     * $tag = XML_Util::createComment('I am a comment');
     * </code>
     *
     * @param string $content content of the comment
     *
     * @return string XML comment
     * @access public
     * @static
     */
    function createComment($content)
    {
        $comment = sprintf('<!-- %s -->', $content);
        return $comment;
    }

    /**
     * create a CData section
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // create a CData section
     * $tag = XML_Util::createCDataSection('I am content.');
     * </code>
     *
     * @param string $data data of the CData section
     *
     * @return string CData section with content
     * @access public
     * @static
     */
    function createCDataSection($data)
    {
        return sprintf('<![CDATA[%s]]>',
            preg_replace('/\]\]>/', ']]]]><![CDATA[>', strval($data)));

    }

    /**
     * split qualified name and return namespace and local part
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // split qualified tag
     * $parts = XML_Util::splitQualifiedName('xslt:stylesheet');
     * </code>
     * the returned array will contain two elements:
     * <pre>
     * array(
     *     'namespace' => 'xslt',
     *     'localPart' => 'stylesheet'
     * );
     * </pre>
     *
     * @param string $qname     qualified tag name
     * @param string $defaultNs default namespace (optional)
     *
     * @return array array containing namespace and local part
     * @access public
     * @static
     */
    function splitQualifiedName($qname, $defaultNs = null)
    {
        if (strstr($qname, ':')) {
            $tmp = explode(':', $qname);
            return array(
                'namespace' => $tmp[0],
                'localPart' => $tmp[1]
            );
        }
        return array(
            'namespace' => $defaultNs,
            'localPart' => $qname
        );
    }

    /**
     * check, whether string is valid XML name
     *
     * <p>XML names are used for tagname, attribute names and various
     * other, lesser known entities.</p>
     * <p>An XML name may only consist of alphanumeric characters,
     * dashes, undescores and periods, and has to start with a letter
     * or an underscore.</p>
     *
     * <code>
     * require_once 'XML/Util.php';
     *
     * // verify tag name
     * $result = XML_Util::isValidName('invalidTag?');
     * if (is_a($result, 'PEAR_Error')) {
     *    print 'Invalid XML name: ' . $result->getMessage();
     * }
     * </code>
     *
     * @param string $string string that should be checked
     *
     * @return mixed true, if string is a valid XML name, PEAR error otherwise
     * @access public
     * @static
     * @todo support for other charsets
     * @todo PEAR CS - unable to avoid 85-char limit on second preg_match
     */
    function isValidName($string)
    {
        // check for invalid chars
        if (!preg_match('/^[[:alpha:]_]\\z/', $string{0})) {
            return XML_Util::raiseError('XML names may only start with letter '
                . 'or underscore', XML_UTIL_ERROR_INVALID_START);
        }

        // check for invalid chars
        if (!preg_match('/^([[:alpha:]_]([[:alnum:]\-\.]*)?:)?[[:alpha:]_]([[:alnum:]\_\-\.]+)?\\z/',
            $string)
        ) {
            return XML_Util::raiseError('XML names may only contain alphanumeric '
                . 'chars, period, hyphen, colon and underscores',
                XML_UTIL_ERROR_INVALID_CHARS);
        }
        // XML name is valid
        return true;
    }

    /**
     * replacement for XML_Util::raiseError
     *
     * Avoids the necessity to always require
     * PEAR.php
     *
     * @param string $msg  error message
     * @param int    $code error code
     *
     * @return PEAR_Error
     * @access public
     * @static
     * @todo PEAR CS - should this use include_once instead?
     */
    function raiseError($msg, $code)
    {
        require_once 'PEAR.php';
        return PEAR::raiseError($msg, $code);
    }
}
?>

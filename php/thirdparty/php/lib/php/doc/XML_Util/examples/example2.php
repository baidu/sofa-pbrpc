<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * Examples (file #2)
 *
 * several examples for the methods of XML_Util
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
 * @category   XML
 * @package    XML_Util
 * @subpackage Examples
 * @author     Stephan Schmidt <schst@php.net>
 * @copyright  2003-2008 Stephan Schmidt <schst@php.net>
 * @license    http://opensource.org/licenses/bsd-license New BSD License
 * @version    CVS: $Id$
 * @link       http://pear.php.net/package/XML_Util
 */

    /**
     * set error level
     */
    error_reporting(E_ALL);

    require_once 'XML/Util.php';

    /**
     * creating a start element
     */
    print 'creating a start element:<br>';
    print htmlentities(XML_Util::createStartElement('myNs:myTag', 
        array('foo' => 'bar'), 'http://www.w3c.org/myNs#'));
    print "\n<br><br>\n";


    /**
     * creating a start element
     */
    print 'creating a start element:<br>';
    print htmlentities(XML_Util::createStartElement('myTag', 
        array(), 'http://www.w3c.org/myNs#'));
    print "\n<br><br>\n";

    /**
     * creating a start element
     */
    print 'creating a start element:<br>';
    print '<pre>';
    print htmlentities(XML_Util::createStartElement('myTag', 
        array('foo' => 'bar', 'argh' => 'tomato'), 
        'http://www.w3c.org/myNs#', true));
    print '</pre>';
    print "\n<br><br>\n";


    /**
     * creating an end element
     */
    print 'creating an end element:<br>';
    print htmlentities(XML_Util::createEndElement('myNs:myTag'));
    print "\n<br><br>\n";

    /**
     * creating a CData section
     */
    print 'creating a CData section:<br>';
    print htmlentities(XML_Util::createCDataSection('I am content.'));
    print "\n<br><br>\n";

    /**
     * creating a comment
     */
    print 'creating a comment:<br>';
    print htmlentities(XML_Util::createComment('I am a comment.'));
    print "\n<br><br>\n";

    /**
     * creating an XML tag with multiline mode
     */
    $tag = array(
        'qname'        => 'foo:bar',
        'namespaceUri' => 'http://foo.com',
        'attributes'   => array('key' => 'value', 'argh' => 'fruit&vegetable'),
        'content'      => 'I\'m inside the tag & contain dangerous chars'
    );

    print 'creating a tag with qualified name and namespaceUri:<br>';
    print '<pre>';
    print htmlentities(XML_Util::createTagFromArray($tag, 
        XML_UTIL_REPLACE_ENTITIES, true));
    print '</pre>';
    print "\n<br><br>\n";

    /**
     * create an attribute string without replacing the entities
     */
    $atts = array('series' => 'Starsky &amp; Hutch', 'channel' => 'ABC');
    print 'creating a attribute string, '
        . 'entities in values already had been replaced:<br>';
    print htmlentities(XML_Util::attributesToString($atts, 
        true, false, false, false, XML_UTIL_ENTITIES_NONE));
    print "\n<br><br>\n";

    /**
     * using the array-syntax for attributesToString()
     */
    $atts = array('series' => 'Starsky &amp; Hutch', 'channel' => 'ABC');
    print 'using the array-syntax for attributesToString()<br>';
    print htmlentities(XML_Util::attributesToString($atts, 
        array('entities' => XML_UTIL_ENTITIES_NONE)));
    print "\n<br><br>\n";


?>

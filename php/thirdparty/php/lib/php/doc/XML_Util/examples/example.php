<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
 * Examples (file #1)
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
     * replacing XML entities
     */
    print 'replace XML entities:<br>';
    print XML_Util::replaceEntities('This string contains < & >.');
    print "\n<br><br>\n";

    /**
     * reversing XML entities
     */
    print 'replace XML entities:<br>';
    print XML_Util::reverseEntities('This string contains &lt; &amp; &gt;.');
    print "\n<br><br>\n";

    /**
     * building XML declaration
     */
    print 'building XML declaration:<br>';
    print htmlspecialchars(XML_Util::getXMLDeclaration());
    print "\n<br><br>\n";

    print 'building XML declaration with additional attributes:<br>';
    print htmlspecialchars(XML_Util::getXMLDeclaration('1.0', 'UTF-8', true));
    print "\n<br><br>\n";

    /**
     * building document type declaration
     */
    print 'building DocType declaration:<br>';
    print htmlspecialchars(XML_Util::getDocTypeDeclaration('package', 
        'http://pear.php.net/dtd/package-1.0'));
    print "\n<br><br>\n";

    print 'building DocType declaration with public ID (does not exist):<br>';
    print htmlspecialchars(XML_Util::getDocTypeDeclaration('package', 
        array('uri' => 'http://pear.php.net/dtd/package-1.0', 
            'id' => '-//PHP//PEAR/DTD PACKAGE 0.1')));
    print "\n<br><br>\n";

    print 'building DocType declaration with internal DTD:<br>';
    print '<pre>';
    print htmlspecialchars(XML_Util::getDocTypeDeclaration('package', 
        'http://pear.php.net/dtd/package-1.0', 
        '<!ELEMENT additionalInfo (#PCDATA)>'));
    print '</pre>';
    print "\n<br><br>\n";

    /**
     * creating an attribute string
     */
    $att = array(
        'foo'  => 'bar',
        'argh' => 'tomato'
    );

    print 'converting array to string:<br>';
    print XML_Util::attributesToString($att);
    print "\n<br><br>\n";


    /**
     * creating an attribute string with linebreaks
     */
    $att = array(
        'foo'  => 'bar',
        'argh' => 'tomato'
    );

    print 'converting array to string (including line breaks):<br>';
    print '<pre>';
    print XML_Util::attributesToString($att, true, true);
    print '</pre>';
    print "\n<br><br>\n";


    /**
     * splitting a qualified tag name
     */
    print 'splitting qualified tag name:<br>';
    print '<pre>';
    print_r(XML_Util::splitQualifiedName('xslt:stylesheet'));
    print '</pre>';
    print "\n<br>\n";


    /**
     * splitting a qualified tag name (no namespace)
     */
    print 'splitting qualified tag name (no namespace):<br>';
    print '<pre>';
    print_r(XML_Util::splitQualifiedName('foo'));
    print '</pre>';
    print "\n<br>\n";

    /**
     * splitting a qualified tag name (no namespace, but default namespace specified)
     */
    print 'splitting qualified tag name '
        . '(no namespace, but default namespace specified):<br>';
    print '<pre>';
    print_r(XML_Util::splitQualifiedName('foo', 'bar'));
    print '</pre>';
    print "\n<br>\n";

    /**
     * verifying XML names
     */
    print 'verifying \'My private tag\':<br>';
    print '<pre>';
    print_r(XML_Util::isValidname('My Private Tag'));
    print '</pre>';
    print "\n<br><br>\n";
    
    print 'verifying \'-MyTag\':<br>';
    print '<pre>';
    print_r(XML_Util::isValidname('-MyTag'));
    print '</pre>';
    print "\n<br><br>\n";

    /**
     * creating an XML tag
     */
    $tag = array(
        'namespace'  => 'foo',
        'localPart'  => 'bar',
        'attributes' => array('key' => 'value', 'argh' => 'fruit&vegetable'),
        'content'    => 'I\'m inside the tag'
    );

    print 'creating a tag with namespace and local part:<br>';
    print htmlentities(XML_Util::createTagFromArray($tag));
    print "\n<br><br>\n";

    /**
     * creating an XML tag
     */
    $tag = array(
        'qname'        => 'foo:bar',
        'namespaceUri' => 'http://foo.com',
        'attributes'   => array('key' => 'value', 'argh' => 'fruit&vegetable'),
        'content'      => 'I\'m inside the tag'
    );

    print 'creating a tag with qualified name and namespaceUri:<br>';
    print htmlentities(XML_Util::createTagFromArray($tag));
    print "\n<br><br>\n";

    /**
     * creating an XML tag
     */
    $tag = array(
        'qname'        => 'bar',
        'namespaceUri' => 'http://foo.com',
        'attributes'   => array('key' => 'value', 'argh' => 'fruit&vegetable')
    );

    print 'creating an empty tag without namespace but namespace Uri:<br>';
    print htmlentities(XML_Util::createTagFromArray($tag));
    print "\n<br><br>\n";

    /**
     * creating an XML tag with more namespaces
     */
    $tag = array(
        'namespace'   => 'foo',
        'localPart'   => 'bar',
        'attributes'  => array('key' => 'value', 'argh' => 'fruit&vegetable'),
        'content'     => 'I\'m inside the tag',
        'namespaces'  => array(
            'bar'  => 'http://bar.com',
            'pear' => 'http://pear.php.net',
        )
    );

    print 'creating an XML tag with more namespaces:<br />';
    print htmlentities(XML_Util::createTagFromArray($tag));
    print "\n<br><br>\n";

    /**
     * creating an XML tag with a CData Section
     */
    $tag = array(
        'qname'      => 'foo',
        'attributes' => array('key' => 'value', 'argh' => 'fruit&vegetable'),
        'content'    => 'I\'m inside the tag'
    );

    print 'creating a tag with CData section:<br>';
    print htmlentities(XML_Util::createTagFromArray($tag, XML_UTIL_CDATA_SECTION));
    print "\n<br><br>\n";

    /**
     * creating an XML tag with a CData Section
     */
    $tag = array(
        'qname'      => 'foo',
        'attributes' => array('key' => 'value', 'argh' => 'tütü'),
        'content'    => 
            'Also XHTML-tags can be created '
            . 'and HTML entities can be replaced Ä ä Ü ö <>.'
    );

    print 'creating a tag with HTML entities:<br>';
    print htmlentities(XML_Util::createTagFromArray($tag, XML_UTIL_ENTITIES_HTML));
    print "\n<br><br>\n";

    /**
    * creating an XML tag with createTag
    */
    print 'creating a tag with createTag:<br>';
    print htmlentities(XML_Util::createTag('myNs:myTag', 
        array('foo' => 'bar'), 
        'This is inside the tag', 
        'http://www.w3c.org/myNs#'));
    print "\n<br><br>\n";

    
    /**
     * trying to create an XML tag with an array as content
     */
    $tag = array(
        'qname'   => 'bar',
        'content' => array('foo' => 'bar')
    );
    print 'trying to create an XML tag with an array as content:<br>';
    print '<pre>';
    print_r(XML_Util::createTagFromArray($tag));
    print '</pre>';
    print "\n<br><br>\n";
    
    /**
     * trying to create an XML tag without a name
     */
    $tag = array(
        'attributes' => array('foo' => 'bar'),
    );
    print 'trying to create an XML tag without a name:<br>';
    print '<pre>';
    print_r(XML_Util::createTagFromArray($tag));
    print '</pre>';
    print "\n<br><br>\n";
?>

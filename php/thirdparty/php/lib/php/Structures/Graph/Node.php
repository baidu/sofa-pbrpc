<?php
/* vim: set expandtab tabstop=4 shiftwidth=4 foldmethod=marker: */
// +-----------------------------------------------------------------------------+
// | Copyright (c) 2003 Sérgio Gonçalves Carvalho                                |
// +-----------------------------------------------------------------------------+
// | This file is part of Structures_Graph.                                      |
// |                                                                             |
// | Structures_Graph is free software; you can redistribute it and/or modify    |
// | it under the terms of the GNU Lesser General Public License as published by |
// | the Free Software Foundation; either version 2.1 of the License, or         |
// | (at your option) any later version.                                         |
// |                                                                             |
// | Structures_Graph is distributed in the hope that it will be useful,         |
// | but WITHOUT ANY WARRANTY; without even the implied warranty of              |
// | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
// | GNU Lesser General Public License for more details.                         |
// |                                                                             |
// | You should have received a copy of the GNU Lesser General Public License    |
// | along with Structures_Graph; if not, write to the Free Software             |
// | Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA                    |
// | 02111-1307 USA                                                              |
// +-----------------------------------------------------------------------------+
// | Author: Sérgio Carvalho <sergio.carvalho@portugalmail.com>                  |
// +-----------------------------------------------------------------------------+
//
/**
 * This file contains the definition of the Structures_Graph_Node class
 * 
 * @see Structures_Graph_Node
 * @package Structures_Graph
 */

/* dependencies {{{ */
/** */
require_once 'PEAR.php';
/** */
require_once 'Structures/Graph.php';
/* }}} */

/* class Structures_Graph_Node {{{ */
/**
 * The Structures_Graph_Node class represents a Node that can be member of a 
 * graph node set.
 *
 * A graph node can contain data. Under this API, the node contains default data, 
 * and key index data. It behaves, thus, both as a regular data node, and as a 
 * dictionary (or associative array) node.
 * 
 * Regular data is accessed via getData and setData. Key indexed data is accessed
 * via getMetadata and setMetadata.
 *
 * @author		Sérgio Carvalho <sergio.carvalho@portugalmail.com> 
 * @copyright	(c) 2004 by Sérgio Carvalho
 * @package Structures_Graph
 */
/* }}} */
class Structures_Graph_Node {
    /* fields {{{ */
    /** 
     * @access private 
     */
    var $_data = null;
    /** @access private */
    var $_metadata = array();
    /** @access private */
    var $_arcs = array();
    /** @access private */
    var $_graph = null;
    /* }}} */

    /* Constructor {{{ */
    /**
    *
    * Constructor
    *
    * @access	public
    */
    function Structures_Graph_Node() {
    }
    /* }}} */

    /* getGraph {{{ */
    /**
    *
    * Node graph getter
    *
    * @return	Structures_Graph	Graph where node is stored
    * @access	public
    */
    function &getGraph() {
        return $this->_graph;
    }
    /* }}} */

    /* setGraph {{{ */
    /**
    *
    * Node graph setter. This method should not be called directly. Use Graph::addNode instead.
    *
    * @param    Structures_Graph   Set the graph for this node. 
    * @see      Structures_Graph::addNode()
    * @access	public
    */
    function setGraph(&$graph) {
        $this->_graph =& $graph;
    }
    /* }}} */

    /* getData {{{ */
    /**
    *
    * Node data getter.
    * 
    * Each graph node can contain a reference to one variable. This is the getter for that reference.
    *
    * @return	mixed	Data stored in node
    * @access	public
    */
    function &getData() {
        return $this->_data;
    }
    /* }}} */

    /* setData {{{ */
    /**
    *
    * Node data setter
    *
    * Each graph node can contain a reference to one variable. This is the setter for that reference.
    *
    * @return	mixed	Data to store in node
    * @access	public
    */
    function setData($data) {
        $this->_data =& $data;
    }
    /* }}} */

    /* metadataKeyExists {{{ */
    /**
    *
    * Test for existence of metadata under a given key.
    *
    * Each graph node can contain multiple 'metadata' entries, each stored under a different key, as in an 
    * associative array or in a dictionary. This method tests whether a given metadata key exists for this node.
    *
    * @param    string    Key to test
    * @return	boolean	 
    * @access	public
    */
    function metadataKeyExists($key) {
        return array_key_exists($key, $this->_metadata);
    }
    /* }}} */

    /* getMetadata {{{ */
    /**
    *
    * Node metadata getter
    *
    * Each graph node can contain multiple 'metadata' entries, each stored under a different key, as in an 
    * associative array or in a dictionary. This method gets the data under the given key. If the key does
    * not exist, an error will be thrown, so testing using metadataKeyExists might be needed.
    *
    * @param    string  Key
    * @param    boolean nullIfNonexistent (defaults to false).
    * @return	mixed	Metadata Data stored in node under given key
    * @see      metadataKeyExists
    * @access	public
    */
    function &getMetadata($key, $nullIfNonexistent = false) {
        if (array_key_exists($key, $this->_metadata)) {
            return $this->_metadata[$key];
        } else {
            if ($nullIfNonexistent) {
                $a = null;
                return $a;
            } else {
                $a = Pear::raiseError('Structures_Graph_Node::getMetadata: Requested key does not exist', STRUCTURES_GRAPH_ERROR_GENERIC);
                return $a;
            }
        }
    }
    /* }}} */

    /* unsetMetadata {{{ */
    /**
    *
    * Delete metadata by key
    *
    * Each graph node can contain multiple 'metadata' entries, each stored under a different key, as in an 
    * associative array or in a dictionary. This method removes any data that might be stored under the provided key.
    * If the key does not exist, no error is thrown, so it is safe using this method without testing for key existence.
    *
    * @param    string  Key
    * @access	public
    */
    function unsetMetadata($key) {
        if (array_key_exists($key, $this->_metadata)) unset($this->_metadata[$key]);
    }
    /* }}} */

    /* setMetadata {{{ */
    /**
    *
    * Node metadata setter
    *
    * Each graph node can contain multiple 'metadata' entries, each stored under a different key, as in an 
    * associative array or in a dictionary. This method stores data under the given key. If the key already exists,
    * previously stored data is discarded.
    *
    * @param    string  Key
    * @param    mixed   Data 
    * @access	public
    */
    function setMetadata($key, $data) {
        $this->_metadata[$key] =& $data;
    }
    /* }}} */

    /* _connectTo {{{ */
    /** @access private */
    function _connectTo(&$destinationNode) {
        $this->_arcs[] =& $destinationNode;
    }
    /* }}} */

    /* connectTo {{{ */
    /**
    *
    * Connect this node to another one.
    * 
    * If the graph is not directed, the reverse arc, connecting $destinationNode to $this is also created.
    *
    * @param    Structures_Graph_Node Node to connect to
    * @access	public
    */
    function connectTo(&$destinationNode) {
        // We only connect to nodes
        if (!is_a($destinationNode, 'Structures_Graph_Node')) return Pear::raiseError('Structures_Graph_Node::connectTo received an object that is not a Structures_Graph_Node', STRUCTURES_GRAPH_ERROR_GENERIC);
        // Nodes must already be in graphs to be connected
        if ($this->_graph == null) return Pear::raiseError('Structures_Graph_Node::connectTo Tried to connect a node that is not in a graph', STRUCTURES_GRAPH_ERROR_GENERIC);
        if ($destinationNode->getGraph() == null) return Pear::raiseError('Structures_Graph_Node::connectTo Tried to connect to a node that is not in a graph', STRUCTURES_GRAPH_ERROR_GENERIC);
        // Connect here
        $this->_connectTo($destinationNode);
        // If graph is undirected, connect back
        if (!$this->_graph->isDirected()) {
            $destinationNode->_connectTo($this);
        }
    }
    /* }}} */

    /* getNeighbours {{{ */
    /**
    *
    * Return nodes connected to this one.
    * 
    * @return   array   Array of nodes
    * @access	public
    */
    function getNeighbours() {
        return $this->_arcs;
    }
    /* }}} */

    /* connectsTo {{{ */
    /**
    *
    * Test wether this node has an arc to the target node
    *
    * @return	boolean   True if the two nodes are connected
    * @access	public
    */
    function connectsTo(&$target) {
        if (version_compare(PHP_VERSION, '5.0.0') >= 0) {
            return in_array($target, $this->getNeighbours(), true);
        }

        $copy = $target;
        $arcKeys = array_keys($this->_arcs);
        foreach($arcKeys as $key) {
            /* ZE1 chokes on this expression:
                if ($target === $arc) return true;
              so, we'll use more convoluted stuff
            */
            $arc =& $this->_arcs[$key];
            $target = true;
            if ($arc === true) {
                $target = false;
                if ($arc === false) {
                    $target = $copy;
                    return true;
                }
            }
        }
        $target = $copy;
        return false;
    }
    /* }}} */

    /* inDegree {{{ */
    /**
    *
    * Calculate the in degree of the node.
    * 
    * The indegree for a node is the number of arcs entering the node. For non directed graphs, 
    * the indegree is equal to the outdegree.
    *
    * @return	integer	 In degree of the node
    * @access	public
    */
    function inDegree() {
        if ($this->_graph == null) return 0;
        if (!$this->_graph->isDirected()) return $this->outDegree();
        $result = 0;
        $graphNodes =& $this->_graph->getNodes();
        foreach (array_keys($graphNodes) as $key) {
            if ($graphNodes[$key]->connectsTo($this)) $result++;
        }
        return $result;
        
    }
    /* }}} */

    /* outDegree {{{ */
    /**
    *
    * Calculate the out degree of the node.
    *
    * The outdegree for a node is the number of arcs exiting the node. For non directed graphs,
    * the outdegree is always equal to the indegree.
    * 
    * @return	integer	 Out degree of the node
    * @access	public
    */
    function outDegree() {
        if ($this->_graph == null) return 0;
        return sizeof($this->_arcs);
    }
    /* }}} */
}
?>

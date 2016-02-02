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
 * This file contains the definition of the Structures_Graph_Manipulator_AcyclicTest graph manipulator.
 * 
 * @see Structures_Graph_Manipulator_AcyclicTest
 * @package Structures_Graph
 */

/* dependencies {{{ */
/** */
require_once 'PEAR.php';
/** */
require_once 'Structures/Graph.php';
/** */
require_once 'Structures/Graph/Node.php';
/* }}} */

/* class Structures_Graph_Manipulator_AcyclicTest {{{ */
/**
 * The Structures_Graph_Manipulator_AcyclicTest is a graph manipulator
 * which tests whether a graph contains a cycle. 
 * 
 * The definition of an acyclic graph used in this manipulator is that of a 
 * DAG. The graph must be directed, or else it is considered cyclic, even when 
 * there are no arcs.
 *
 * @author		Sérgio Carvalho <sergio.carvalho@portugalmail.com> 
 * @copyright	(c) 2004 by Sérgio Carvalho
 * @package Structures_Graph
 */
class Structures_Graph_Manipulator_AcyclicTest {
    /* _nonVisitedInDegree {{{ */
    /**
    *
    * This is a variant of Structures_Graph::inDegree which does 
    * not count nodes marked as visited.
    *
    * @access   private
    * @return	integer	 Number of non-visited nodes that link to this one
    */
    function _nonVisitedInDegree(&$node) {
        $result = 0;
        $graphNodes =& $node->_graph->getNodes();
        foreach (array_keys($graphNodes) as $key) {
            if ((!$graphNodes[$key]->getMetadata('acyclic-test-visited')) && $graphNodes[$key]->connectsTo($node)) $result++;
        }
        return $result;
        
    }
    /* }}} */

    /* _isAcyclic {{{ */
    /**
    * @access   private
    */
    function _isAcyclic(&$graph) {
        // Mark every node as not visited
        $nodes =& $graph->getNodes();
        $nodeKeys = array_keys($nodes);
        $refGenerator = array();
        foreach($nodeKeys as $key) {
            $refGenerator[] = false;
            $nodes[$key]->setMetadata('acyclic-test-visited', $refGenerator[sizeof($refGenerator) - 1]);
        }

        // Iteratively peel off leaf nodes
        do {
            // Find out which nodes are leafs (excluding visited nodes)
            $leafNodes = array();
            foreach($nodeKeys as $key) {
                if ((!$nodes[$key]->getMetadata('acyclic-test-visited')) && Structures_Graph_Manipulator_AcyclicTest::_nonVisitedInDegree($nodes[$key]) == 0) {
                    $leafNodes[] =& $nodes[$key];
                }
            }
            // Mark leafs as visited
            for ($i=sizeof($leafNodes) - 1; $i>=0; $i--) {
                $visited =& $leafNodes[$i]->getMetadata('acyclic-test-visited');
                $visited = true;
                $leafNodes[$i]->setMetadata('acyclic-test-visited', $visited);
            }
        } while (sizeof($leafNodes) > 0);

        // If graph is a DAG, there should be no non-visited nodes. Let's try to prove otherwise
        $result = true;
        foreach($nodeKeys as $key) if (!$nodes[$key]->getMetadata('acyclic-test-visited')) $result = false;
        
        // Cleanup visited marks
        foreach($nodeKeys as $key) $nodes[$key]->unsetMetadata('acyclic-test-visited');

        return $result;
    }
    /* }}} */

    /* isAcyclic {{{ */
    /**
    *
    * isAcyclic returns true if a graph contains no cycles, false otherwise.
    *
    * @return	boolean	 true iff graph is acyclic
    * @access	public
    */
    function isAcyclic(&$graph) {
        // We only test graphs
        if (!is_a($graph, 'Structures_Graph')) return Pear::raiseError('Structures_Graph_Manipulator_AcyclicTest::isAcyclic received an object that is not a Structures_Graph', STRUCTURES_GRAPH_ERROR_GENERIC);
        if (!$graph->isDirected()) return false; // Only directed graphs may be acyclic

        return Structures_Graph_Manipulator_AcyclicTest::_isAcyclic($graph);
    }
    /* }}} */
}
/* }}} */
?>

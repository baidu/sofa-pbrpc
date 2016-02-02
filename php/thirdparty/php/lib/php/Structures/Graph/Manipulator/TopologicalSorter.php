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
 * This file contains the definition of the Structures_Graph_Manipulator_TopologicalSorter class.
 * 
 * @see Structures_Graph_Manipulator_TopologicalSorter
 * @package Structures_Graph
 */

/* dependencies {{{ */
/** */
require_once 'PEAR.php';
/** */
require_once 'Structures/Graph.php';
/** */
require_once 'Structures/Graph/Node.php';
/** */
require_once 'Structures/Graph/Manipulator/AcyclicTest.php';
/* }}} */

/* class Structures_Graph_Manipulator_TopologicalSorter {{{ */
/**
 * The Structures_Graph_Manipulator_TopologicalSorter is a manipulator 
 * which is able to return the set of nodes in a graph, sorted by topological 
 * order.
 *
 * A graph may only be sorted topologically iff it's a DAG. You can test it
 * with the Structures_Graph_Manipulator_AcyclicTest.
 * 
 * @author		Sérgio Carvalho <sergio.carvalho@portugalmail.com> 
 * @copyright	(c) 2004 by Sérgio Carvalho
 * @see     Structures_Graph_Manipulator_AcyclicTest
 * @package Structures_Graph
 */
class Structures_Graph_Manipulator_TopologicalSorter {
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
            if ((!$graphNodes[$key]->getMetadata('topological-sort-visited')) && $graphNodes[$key]->connectsTo($node)) $result++;
        }
        return $result;
        
    }
    /* }}} */

    /* _sort {{{ */
    /**
    * @access   private
    */
    function _sort(&$graph) {
        // Mark every node as not visited
        $nodes =& $graph->getNodes();
        $nodeKeys = array_keys($nodes);
        $refGenerator = array();
        foreach($nodeKeys as $key) {
            $refGenerator[] = false;
            $nodes[$key]->setMetadata('topological-sort-visited', $refGenerator[sizeof($refGenerator) - 1]);
        }

        // Iteratively peel off leaf nodes
        $topologicalLevel = 0;
        do {
            // Find out which nodes are leafs (excluding visited nodes)
            $leafNodes = array();
            foreach($nodeKeys as $key) {
                if ((!$nodes[$key]->getMetadata('topological-sort-visited')) && Structures_Graph_Manipulator_TopologicalSorter::_nonVisitedInDegree($nodes[$key]) == 0) {
                    $leafNodes[] =& $nodes[$key];
                }
            }
            // Mark leafs as visited
            $refGenerator[] = $topologicalLevel;
            for ($i=sizeof($leafNodes) - 1; $i>=0; $i--) {
                $visited =& $leafNodes[$i]->getMetadata('topological-sort-visited');
                $visited = true;
                $leafNodes[$i]->setMetadata('topological-sort-visited', $visited);
                $leafNodes[$i]->setMetadata('topological-sort-level', $refGenerator[sizeof($refGenerator) - 1]);
            }
            $topologicalLevel++;
        } while (sizeof($leafNodes) > 0);

        // Cleanup visited marks
        foreach($nodeKeys as $key) $nodes[$key]->unsetMetadata('topological-sort-visited');
    }
    /* }}} */

    /* sort {{{ */
    /**
    *
    * sort returns the graph's nodes, sorted by topological order. 
    * 
    * The result is an array with 
    * as many entries as topological levels. Each entry in this array is an array of nodes within
    * the given topological level.
    *
    * @return	array	 The graph's nodes, sorted by topological order.
    * @access	public
    */
    function sort(&$graph) {
        // We only sort graphs
        if (!is_a($graph, 'Structures_Graph')) return Pear::raiseError('Structures_Graph_Manipulator_TopologicalSorter::sort received an object that is not a Structures_Graph', STRUCTURES_GRAPH_ERROR_GENERIC);
        if (!Structures_Graph_Manipulator_AcyclicTest::isAcyclic($graph)) return Pear::raiseError('Structures_Graph_Manipulator_TopologicalSorter::sort received an graph that has cycles', STRUCTURES_GRAPH_ERROR_GENERIC);

        Structures_Graph_Manipulator_TopologicalSorter::_sort($graph);
        $result = array();

        // Fill out result array
        $nodes =& $graph->getNodes();
        $nodeKeys = array_keys($nodes);
        foreach($nodeKeys as $key) {
            if (!array_key_exists($nodes[$key]->getMetadata('topological-sort-level'), $result)) $result[$nodes[$key]->getMetadata('topological-sort-level')] = array();
            $result[$nodes[$key]->getMetadata('topological-sort-level')][] =& $nodes[$key];
            $nodes[$key]->unsetMetadata('topological-sort-level');
        }

        return $result;
    }
    /* }}} */
}
/* }}} */
?>

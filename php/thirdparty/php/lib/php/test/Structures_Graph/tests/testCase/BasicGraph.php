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

require_once 'Structures/Graph.php';
require_once 'PHPUnit/Framework.php';

/**
 * @access private
 */
class BasicGraph extends PHPUnit_Framework_TestCase
{
    var $_graph = null;

    function test_create_graph() {
        $this->_graph = new Structures_Graph();
        $this->assertTrue(is_a($this->_graph, 'Structures_Graph')); 
    }

    function test_add_node() {
        $this->_graph = new Structures_Graph();
        $data = 1;
        $node = new Structures_Graph_Node($data);
        $this->_graph->addNode($node);
        $node = new Structures_Graph_Node($data);
        $this->_graph->addNode($node);
        $node = new Structures_Graph_Node($data);
        $this->_graph->addNode($node);
    }

    function test_connect_node() {
        $this->_graph = new Structures_Graph();
        $data = 1;
        $node1 = new Structures_Graph_Node($data);
        $node2 = new Structures_Graph_Node($data);
        $this->_graph->addNode($node1);
        $this->_graph->addNode($node2);
        $node1->connectTo($node2);

        $node =& $this->_graph->getNodes();
        $node =& $node[0];
        $node = $node->getNeighbours();
        $node =& $node[0];
        /* 
         ZE1 == and === operators fail on $node,$node2 because of the recursion introduced
         by the _graph field in the Node object. So, we'll use the stupid method for reference
         testing
        */
        $node = true;
        $this->assertTrue($node2);
        $node = false;
        $this->assertFalse($node2);
    }

    function test_data_references() {
        $this->_graph = new Structures_Graph();
        $data = 1;
        $node = new Structures_Graph_Node();
        $node->setData(&$data);
        $this->_graph->addNode($node);
        $data = 2;
        $dataInNode =& $this->_graph->getNodes();
        $dataInNode =& $dataInNode[0];
        $dataInNode =& $dataInNode->getData();
        $this->assertTrue($data === $dataInNode);
    }

    function test_metadata_references() {
        $this->_graph = new Structures_Graph();
        $data = 1;
        $node = new Structures_Graph_Node();
        $node->setMetadata('5', &$data);
        $data = 2;
        $dataInNode =& $node->getMetadata('5');
        $this->assertTrue($data === $dataInNode);
    }
   
    function test_metadata_key_exists() {
        $this->_graph = new Structures_Graph();
        $data = 1;
        $node = new Structures_Graph_Node();
        $node->setMetadata('5', $data);
        $this->assertTrue($node->metadataKeyExists('5'));
        $this->assertFalse($node->metadataKeyExists('1'));
    }

    function test_directed_degree() {
        $this->_graph = new Structures_Graph(true);
        $node = array();
        $node[] = new Structures_Graph_Node();
        $node[] = new Structures_Graph_Node();
        $node[] = new Structures_Graph_Node();
        $this->_graph->addNode($node[0]);
        $this->_graph->addNode($node[1]);
        $this->_graph->addNode($node[2]);
        $this->assertEquals(0, $node[0]->inDegree(), 'inDegree test failed for node 0 with 0 arcs');
        $this->assertEquals(0, $node[1]->inDegree(), 'inDegree test failed for node 1 with 0 arcs');
        $this->assertEquals(0, $node[2]->inDegree(), 'inDegree test failed for node 2 with 0 arcs');
        $this->assertEquals(0, $node[0]->outDegree(), 'outDegree test failed for node 0 with 0 arcs');
        $this->assertEquals(0, $node[1]->outDegree(), 'outDegree test failed for node 1 with 0 arcs');
        $this->assertEquals(0, $node[2]->outDegree(), 'outDegree test failed for node 2 with 0 arcs');
        $node[0]->connectTo($node[1]);
        $this->assertEquals(0, $node[0]->inDegree(), 'inDegree test failed for node 0 with 1 arc');
        $this->assertEquals(1, $node[1]->inDegree(), 'inDegree test failed for node 1 with 1 arc');
        $this->assertEquals(0, $node[2]->inDegree(), 'inDegree test failed for node 2 with 1 arc');
        $this->assertEquals(1, $node[0]->outDegree(), 'outDegree test failed for node 0 with 1 arc');
        $this->assertEquals(0, $node[1]->outDegree(), 'outDegree test failed for node 1 with 1 arc');
        $this->assertEquals(0, $node[2]->outDegree(), 'outDegree test failed for node 2 with 1 arc');
        $node[0]->connectTo($node[2]);
        $this->assertEquals(0, $node[0]->inDegree(), 'inDegree test failed for node 0 with 2 arcs');
        $this->assertEquals(1, $node[1]->inDegree(), 'inDegree test failed for node 1 with 2 arcs');
        $this->assertEquals(1, $node[2]->inDegree(), 'inDegree test failed for node 2 with 2 arcs');
        $this->assertEquals(2, $node[0]->outDegree(), 'outDegree test failed for node 0 with 2 arcs');
        $this->assertEquals(0, $node[1]->outDegree(), 'outDegree test failed for node 1 with 2 arcs');
        $this->assertEquals(0, $node[2]->outDegree(), 'outDegree test failed for node 2 with 2 arcs');
    }

    function test_undirected_degree() {
        $this->_graph = new Structures_Graph(false);
        $node = array();
        $node[] = new Structures_Graph_Node();
        $node[] = new Structures_Graph_Node();
        $node[] = new Structures_Graph_Node();
        $this->_graph->addNode($node[0]);
        $this->_graph->addNode($node[1]);
        $this->_graph->addNode($node[2]);
        $this->assertEquals(0, $node[0]->inDegree(), 'inDegree test failed for node 0 with 0 arcs');
        $this->assertEquals(0, $node[1]->inDegree(), 'inDegree test failed for node 1 with 0 arcs');
        $this->assertEquals(0, $node[2]->inDegree(), 'inDegree test failed for node 2 with 0 arcs');
        $this->assertEquals(0, $node[0]->outDegree(), 'outDegree test failed for node 0 with 0 arcs');
        $this->assertEquals(0, $node[1]->outDegree(), 'outDegree test failed for node 1 with 0 arcs');
        $this->assertEquals(0, $node[2]->outDegree(), 'outDegree test failed for node 2 with 0 arcs');
        $node[0]->connectTo($node[1]);
        $this->assertEquals(1, $node[0]->inDegree(), 'inDegree test failed for node 0 with 1 arc');
        $this->assertEquals(1, $node[1]->inDegree(), 'inDegree test failed for node 1 with 1 arc');
        $this->assertEquals(0, $node[2]->inDegree(), 'inDegree test failed for node 2 with 1 arc');
        $this->assertEquals(1, $node[0]->outDegree(), 'outDegree test failed for node 0 with 1 arc');
        $this->assertEquals(1, $node[1]->outDegree(), 'outDegree test failed for node 1 with 1 arc');
        $this->assertEquals(0, $node[2]->outDegree(), 'outDegree test failed for node 2 with 1 arc');
        $node[0]->connectTo($node[2]);
        $this->assertEquals(2, $node[0]->inDegree(), 'inDegree test failed for node 0 with 2 arcs');
        $this->assertEquals(1, $node[1]->inDegree(), 'inDegree test failed for node 1 with 2 arcs');
        $this->assertEquals(1, $node[2]->inDegree(), 'inDegree test failed for node 2 with 2 arcs');
        $this->assertEquals(2, $node[0]->outDegree(), 'outDegree test failed for node 0 with 2 arcs');
        $this->assertEquals(1, $node[1]->outDegree(), 'outDegree test failed for node 1 with 2 arcs');
        $this->assertEquals(1, $node[2]->outDegree(), 'outDegree test failed for node 2 with 2 arcs');
    }
}
?>

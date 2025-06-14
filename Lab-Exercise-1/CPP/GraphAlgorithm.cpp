//===- GraphTraversal.cpp -- Graph algorithms ------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2022>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
/*
 * Graph reachability and constraint solving on a self-defined graph
 *
 * Created on: Feb 18, 2024
 */

#include "GraphAlgorithm.h"
#include <unordered_set> // std::unordered_set
#include <functional> // std::function

using namespace std;

/// TODO: Implement your depth-first search here to traverse each program path from src to dst (each node appears at
/// most once in each path). Add each path as a string into std::set<std::string> paths. Each path should have a format
/// like this: "START->1->2->4->5->END", where -> indicates an edge connecting two node IDs.
void Graph::reachability(Node* src, Node* dst) {
	/// TODO: your code starts from here

	// clear member variables to prevent potential call to reachability() on the same graph with different src and dst
	paths.clear(); // a collecetion of "START->1->2->4->5->END"s
	visited.clear(); // store visited nodes to avoid cycles
	path.clear(); // a path as a sequence of node IDs {1,2,4,5}

	// [&] captures all local variables by reference
	std::function<void(Node*)> dfs = [&](Node* curr) {
		visited.insert(curr); // mark visited
		path.push_back(curr->getNodeID()); // add node ID to path

		// base case of recurrsion: destination reached
		if (curr == dst) {
			// construct path string for storage
			std::string pathStr = "START";
			for (auto id : path) {
				pathStr += "->" + std::to_string(id);
			}
			pathStr += "->END";
			paths.insert(pathStr);
		}
		else {
			// recurse on every unvisited neighbour
			for (auto edge : curr->getOutEdges()) {
				auto neighbor = edge->getDst();
				if (!visited.count(neighbor)) {
					dfs(neighbor);
				}
			}
		}

		visited.erase(curr);
		path.pop_back();
	};

	dfs(src);
}

/// TODO: Implement constraint solving by iteratively (1) propagating points-to sets among nodes on CGraph, and (2)
/// adding new copy edges until a fixed point is reached (i.e., no new copy edges are added).
/// The solving rules are as follows:
/// p <--ADDR-- o   =>  pts(p) = pts(p) ∪ {o}
/// q <--COPY-- p   =>  pts(q) = pts(q) ∪ pts(p)
/// q <--LOAD-- p   =>  for each o ∈ pts(p) : q <--COPY-- o
/// q <--STORE-- p  =>  for each o ∈ pts(q) : o <--COPY-- p
/// pts(q) denotes the points-to set of node q.
/// Refer to the APIs in CGraph, including `addPts`, `getPts`, `unionPts` and `addEdge` for your implementation.
void CGraph::solveWorklist() {
	/// TODO: your code starts from here
}
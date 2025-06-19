//===- Assignment-1.cpp -- Taint analysis ------------------//
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
 * Graph reachability, Andersen's pointer analysis and taint analysis
 *
 * Created on: Feb 18, 2024
 */

#include "Assignment_1.h"
#include "WPA/Andersen.h"
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <queue> // for std::queue
#include <sstream>
#include <string>

using namespace SVF;
using namespace llvm;
using namespace std;

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path
/// by matching calls and returns while maintaining a `callstack`.
/// Sources and sinks are identified by implementing and calling `readSrcSnkFromFile`
/// Each path including loops, qualified by a `callstack`, should only be traversed once using a `visited` set.
/// You will need to collect each path from src to snk and then add the path to the `paths` set.
/// Add each path (a sequence of node IDs) as a string into std::set<std::string> paths
/// in the format "START->1->2->4->5->END", where -> indicate an ICFGEdge connects two ICFGNode IDs
void ICFGTraversal::reachability(const ICFGNode* curNode, const ICFGNode* snk) {
	/// TODO: your code starts from here

	ICFGNodeCallStackPair pair = {curNode, callstack};

	// If this node and callstack combination was visited, skip it
	if (visited.find(pair) != visited.end())
		return;
	visited.insert(pair);

	// Add current node ID to the path
	path.push_back(curNode->getId());

	// Reached sink
	if (curNode == snk) {
		std::string collectedPath = "START";
		for (unsigned id : path) {
			collectedPath += "->" + std::to_string(id);
		}
		collectedPath += "->END";
		paths.insert(collectedPath);
	}

	// Traverse outgoing edges
	for (const ICFGEdge* edge : curNode->getOutEdges()) {
		const ICFGNode* dst = edge->getDstNode();

		if (edge->isIntraCFGEdge()) {
			reachability(dst, snk);
		}
		else if (edge->isCallCFGEdge()) {
			const CallCFGEdge* callEdge = SVFUtil::dyn_cast<CallCFGEdge>(edge);
			const CallICFGNode* callSite = callEdge->getCallSite();
			callstack.push_back(callSite);
			reachability(dst, snk);
			callstack.pop_back();
		}
		else if (edge->isRetCFGEdge()) {
			const RetCFGEdge* retEdge = SVFUtil::dyn_cast<RetCFGEdge>(edge);
			const CallICFGNode* callSite = retEdge->getCallSite();
			if (!callstack.empty() && callstack.back() == callSite) {
				callstack.pop_back();
				reachability(dst, snk);
				callstack.push_back(callSite); // Restore after returning
			}
			else if (callstack.empty()) {
				reachability(dst, snk);
			}
		}
	}

	visited.erase(pair); // Backtrack
	path.pop_back(); // Backtrack path
}

/// TODO: Implement your code to parse the two lines to identify sources and sinks from `SrcSnk.txt` for your
/// reachability analysis The format in SrcSnk.txt is in the form of
/// line 1 for sources  "{ api1 api2 api3 }"
/// line 2 for sinks    "{ api1 api2 api3 }"
void ICFGTraversal::readSrcSnkFromFile(const string& filename) {
	/// TODO: your code starts from here

	std::ifstream infile(filename);
	string line;
	int lineNum = 0;

	while (std::getline(infile, line)) {
		std::stringstream ss(line); // convert filestream into stream stream
		std::string token; // a group of chars seperated by spaces

		// Skip label like "source ->" or "sink ->"
		ss >> token; // source/sink
		ss >> token; // ->

		// Convert the rest of ss into a singe line
		std::string restOfLine;
		std::getline(ss, restOfLine);

		// Remove braces
		restOfLine.erase(std::remove(restOfLine.begin(), restOfLine.end(), '{'), restOfLine.end());
		restOfLine.erase(std::remove(restOfLine.begin(), restOfLine.end(), '}'), restOfLine.end());

		// Convert the rest of line into ss of API(function) names seprated by spaces
		std::stringstream apiStream(restOfLine);
		std::string api;

		// read every api's name
		while (apiStream >> api) {
			// separate handling for the first line (source) and second line (sink)
			if (lineNum == 0) {
				checker_source_api.insert(api);
			}
			else if (lineNum == 1) {
				checker_sink_api.insert(api);
			}
		}
		++lineNum;
	}
}

/// TODO: Implement your Andersen's Algorithm here
/// The solving rules are as follows:
/// p <--Addr-- o        =>  pts(p) = pts(p) ∪ {o}
/// q <--COPY-- p        =>  pts(q) = pts(q) ∪ pts(p)
/// q <--LOAD-- p        =>  for each o ∈ pts(p) : q <--COPY-- o
/// q <--STORE-- p       =>  for each o ∈ pts(q) : o <--COPY-- p
/// q <--GEP, fld-- p    =>  for each o ∈ pts(p) : pts(q) = pts(q) ∪ {o.fld}
/// pts(q) denotes the points-to set of q
void AndersenPTA::solveWorklist() {
	/// TODO: your code starts from here

	// === Initialization Phase: Addr edges ===
	for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end(); nodeIt != nodeEit; nodeIt++)
	{
		ConstraintNode* cgNode = nodeIt->second;

		for (const auto* addrOutEdge : cgNode->getAddrOutEdges()) {
			NodeID o = addrOutEdge->getSrcID();
			NodeID p = addrOutEdge->getDstID();
			if (addPts(p, o)) {
				pushIntoWorklist(p);
			}
		}
	}

	// === Main Worklist Propagation ===
	while (!isWorklistEmpty()) {
		NodeID p = popFromWorklist();
		const PointsTo& ptsP = getPts(p);

		for (NodeID o : ptsP) {
			// === Store rule: for q --Store--> p, create q --Copy--> o for all o in pts(p) ===
			for (const ConstraintEdge* storeInEdge : consCG->getConstraintNode(p)->getStoreInEdges()) {
				NodeID q = storeInEdge->getSrcID();

				if (addCopyEdge(q, o)) {
					pushIntoWorklist(q);
				}
			}

			// === Load rule: for p --Load--> r, create o --Copy--> r for all o in pts(p) ===
			for (const ConstraintEdge* loadOutEdge : consCG->getConstraintNode(p)->getLoadOutEdges()) {
				NodeID r = loadOutEdge->getDstID();

				if (addCopyEdge(o, r)) {
					pushIntoWorklist(o);
				}
			}
		}

		// === Copy rule: pts(x) ∪= pts(p) for p --Copy--> x ===
		for (const ConstraintEdge* copyOutEdge : consCG->getConstraintNode(p)->getCopyOutEdges()) {
			NodeID x = copyOutEdge->getDstID();
			if (unionPts(x, ptsP)) {
				pushIntoWorklist(x);
			}
		}

		// === Gep rule: pts(x) ∪= { o.fld | o ∈ pts(p) } for p --Gep,fld--> x ===
		for (const ConstraintEdge* gepOutEdge : consCG->getConstraintNode(p)->getGepOutEdges()) {
			const NormalGepCGEdge* normalGepEdge = SVFUtil::dyn_cast<NormalGepCGEdge>(gepOutEdge);
			NodeID x = normalGepEdge->getDstID();

			bool changed = false;
			for (NodeID o : ptsP) {
				NodeID fldObj = consCG->getGepObjVar(o, normalGepEdge->getConstantFieldIdx());
				if (addPts(x, fldObj)) {
					changed = true;
				}
			}

			if (changed) {
				pushIntoWorklist(x);
			}
		}
	}
}

/// TODO: Checking aliases of the two variables at source and sink. For example:
/// src instruction:  actualRet = source();
/// snk instruction:  sink(actualParm,...);
/// return true if actualRet is aliased with any parameter at the snk node (e.g., via ander->alias(..,..))
bool ICFGTraversal::aliasCheck(const CallICFGNode* src, const CallICFGNode* snk) {
	/// TODO: your code starts from here

	const SVFVar* actualRet = src->getRetICFGNode()->getActualRet();
	const CallICFGNode::ActualParmNodeVec actualParms = snk->getActualParms();

	// Check all actual arguments to the sink
	for (const auto parm : actualParms) {
		if (ander->alias(actualRet, parm)) {
			return true;
		}
	}

	return false;
}

// Start taint checking.
// There is a tainted flow from p@source to q@sink
// if (1) alias(p,q)==true and (2) source reaches sink on ICFG.
void ICFGTraversal::taintChecking() {
	const fs::path& config = CUR_DIR() / "../Tests/SrcSnk.txt";
	// configure sources and sinks for taint analysis
	readSrcSnkFromFile(config);

	// Set file permissions to read-only for user, group and others
	if (chmod(config.string().c_str(), S_IRUSR | S_IRGRP | S_IROTH) == -1) {
		std::cerr << "Error setting file permissions for " << config << ": " << std::strerror(errno) << std::endl;
		abort();
	}
	ander = new AndersenPTA(pag);
	ander->analyze();
	for (const CallICFGNode* src : identifySources()) {
		for (const CallICFGNode* snk : identifySinks()) {
			if (aliasCheck(src, snk))
				reachability(src, snk);
		}
	}
}

/*!
 * Andersen analysis
 */
void AndersenPTA::analyze() {
	initialize();
	initWorklist();
	do {
		reanalyze = false;
		solveWorklist();
		if (updateCallGraph(getIndirectCallsites()))
			reanalyze = true;
	} while (reanalyze);
	finalize();
}

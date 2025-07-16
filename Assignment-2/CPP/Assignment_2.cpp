//===- Assignment-2.cpp -- Automated assertion-based verification (Static symbolic execution) --//
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
 * Automated assertion-based verification (Static symbolic execution)
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_2.h"
#include "Util/Options.h"

using namespace SVF;
using namespace SVFUtil;
using namespace llvm;
using namespace z3;

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from
/// You will need to collect each path from src node to snk node and then add the path to the `paths` set by
/// calling the `collectAndTranslatePath` method, in which translatePath method is called.
/// This implementation, slightly different from Assignment-1, requires ICFGNode* as the first argument.
void SSE::reachability(const ICFGEdge* curEdge, const ICFGNode* snk) {
	/// TODO: your code starts from here

	ICFGEdgeStackPair pair = {curEdge, callingCtx};

	// If this edge + context already visited, skip
	if (visited.find(pair) != visited.end())
		return;
	visited.insert(pair);

	path.push_back(curEdge); // Add current edge to path

	// If destination node of this edge is the sink node, collect path
	if (curEdge->getDstNode() == snk) {
		collectAndTranslatePath();
	}
	else {
		// Traverse all outgoing edges from the destination node
		for (const ICFGEdge* nextEdge : curEdge->getDstNode()->getOutEdges()) {
			// Depending on edge type (Intra, Call, Ret), update context accordingly

			if (nextEdge->isIntraCFGEdge()) {
				// Just continue traversal on intra edges
				reachability(nextEdge, snk);
			}
			else if (nextEdge->isCallCFGEdge()) {
				// On call edges, push callsite into context
				const CallCFGEdge* callEdge = SVFUtil::dyn_cast<CallCFGEdge>(nextEdge);
				if (!callEdge)
					continue;
				const CallICFGNode* callSite = callEdge->getCallSite();

				pushCallingCtx(callSite);
				reachability(nextEdge, snk);
				popCallingCtx();
			}
			else if (nextEdge->isRetCFGEdge()) {
				const RetCFGEdge* retEdge = SVFUtil::dyn_cast<RetCFGEdge>(nextEdge);
				if (!retEdge)
					continue;
				const CallICFGNode* callSite = retEdge->getCallSite();

				if (!callstack.empty() && callstack.back() == callSite) {
					callstack.pop_back();
					reachability(nextEdge, snk);
					callstack.push_back(callSite);
				}
				else if (callstack.empty()) {
					reachability(nextEdge, snk);
				}
			}
		}
	}

	path.pop_back(); // Backtrack path
	visited.erase(pair); // Backtrack visited set
}

/// TODO: collect each path once this method is called during reachability analysis, and
/// Collect each program path from the entry to each assertion of the program. In this function,
/// you will need (1) add each path into the paths set, (2) call translatePath to convert each path into Z3 expressions.
/// Note that translatePath returns true if the path is feasible, false if the path is infeasible. (3) If a path is
/// feasible, you will need to call assertchecking to verify the assertion (which is the last ICFGNode of this path).
void SSE::collectAndTranslatePath() {
	/// TODO: your code starts from here

	// Convert the path (vector of edges) to a string key
	std::stringstream ss;
	ss << "START";
	for (const ICFGEdge* e : path) {
		if (e->getSrcNode()) {
			ss << "->" << e->getSrcNode()->getId();
		}
		else {
			ss << "->START"; // Or skip, or print placeholder
		}
		ss << "->" << e->getDstNode()->getId();
	}

	ss << "->END";
	std::string pathStr = ss.str();

	// Insert path string into the set of paths
	paths.insert(pathStr);

	// Translate the path to Z3 constraints
	bool feasible = translatePath(path);

	// If path is feasible, check assertion at the last node
	if (feasible) {
		const ICFGNode* lastNode = path.back()->getDstNode();
		assertchecking(lastNode);
	}
}

/// TODO: Implement handling of function calls
/// @brief Handles context-sensitive interprocedural constraints at function calls
///        For each parameter edge `lhs = rhs` in a CallCFGEdge (i.e., CallPE),
///        we enforce the equivalence of the actual parameter (at the callsite)
///        and the formal parameter (in the callee) under appropriate contexts.
///
///        That is, for call f(..., q, ...), f(..., p, ...), we encode:
///              ⟨[callerCtx], q⟩ == ⟨[calleeCtx], p⟩
void SSE::handleCall(const CallCFGEdge* calledge) {
	/// TODO: your code starts from here

	// Iterate over all CallPEs associated with this call edge.
	// Each CallPE represents a dataflow edge: formal = actual (parameter assignment).
	for (const auto callPE : calledge->getCallPEs()) {
		// // const CallPE* callPE = SVFUtil::dyn_cast<CallPE>(stmt);
		// if (!callPE)
		// 	continue; // Skip if not a valid CallPE

		// // Save the current calling context (c), which is the caller's context
		// const CallStack callerCtx = callingCtx;

		// Push the current callsite (ICFGNode) onto the call stack to form callee context c'
		// This reflects that we're symbolically moving into the callee function
		pushCallingCtx(callPE->getCallSite());

		// Retrieve variable IDs for the formal and actual parameters
		const auto lhsVarID = callPE->getLHSVarID(); // Formal parameter in callee (under c')
		const auto rhsVarID = callPE->getRHSVarID(); // Actual parameter in caller (under c)

		// Get the Z3 expression for lhs (formal parameter) under the callee's context
		// callingCtx has been updated by pushCallingCtx(), so this is ⟨[c'], lhs⟩
		const auto lhsExpr = getZ3Expr(lhsVarID);

		// Restore the caller context by popping the callsite from the stack
		popCallingCtx();

		// Now callingCtx is back to c (caller context)
		// Get the Z3 expression for rhs (actual parameter) under the caller's context
		const auto rhsExpr = getZ3Expr(rhsVarID);

		// Add constraint to solver: [calleeCtx] lhs == [callerCtx] rhs
		// Meaning the formal parameter in the callee must equal the actual argument in the caller
		addToSolver(lhsExpr == rhsExpr);
	}
}

/// TODO: Implement handling of function returns
/// @brief Handles context-sensitive return value constraints at function returns.
///        Each RetPE corresponds to an assignment like r = return z from the callee,
///        where `r` is a variable in the caller and `z` is the return value in the callee.
///
///        This models the relationship:
///              ⟨[callerCtx], r⟩ == ⟨[calleeCtx], z⟩
///        by using the current callingCtx stack to push/pop the callsite and retrieve
///        expressions under correct contexts.
void SSE::handleRet(const RetCFGEdge* retEdge) {
	/// TODO: your code starts from here

	// Retrieve the unique RetPE (return parameter edge) from this return edge
	const auto retPE = retEdge->getRetPE();
	// if (!retPE)
	// 	return; // Skip if there's no return edge (should not happen in well-formed CFG)

	// // Save the current calling context (callee's context), denoted as c'
	// // This is the context within the callee function
	// const CallStack calleeCtx = callingCtx;

	// Pop the most recent callsite off the context stack to return to the caller's context
	// Now callingCtx represents the caller's context (denoted as c)
	popCallingCtx();

	// Retrieve variable IDs for return assignment:
	// LHS: receiving variable in caller (e.g., r)
	// RHS: return value variable from callee (e.g., z)
	const auto lhsVarID = retPE->getLHSVarID(); // r, in caller
	const auto rhsVarID = retPE->getRHSVarID(); // z, in callee

	// At this point callingCtx is caller context c
	// Retrieve the LHS expression under the caller context
	const auto lhsExpr = getZ3Expr(lhsVarID);

	// Push the callsite back onto the context stack to restore callee context c'
	// (because we need the RHS value under the callee context)
	pushCallingCtx(retPE->getCallSite());

	// Now callingCtx is back to callee context c'
	// Retrieve the RHS expression under the callee context
	const auto rhsExpr = getZ3Expr(rhsVarID);

	// Restore callingCtx back to the caller context after we're done
	popCallingCtx();

	// Add constraint: [callerCtx] lhs == [calleeCtx] rhs
	// This models return value passing from callee to caller
	addToSolver(lhsExpr == rhsExpr);
}

/// TODO: Implement handling of branch statements inside a function
/// Return true if the path is feasible, false otherwise.
/// A given if/else branch on the ICFG looks like the following:
///       	     ICFGNode1 (condition %cmp)
///       	     1	/    \  0
///       	  ICFGNode2   ICFGNode3
/// edge->getCondition() returns the branch condition variable (%cmp) of type SVFValue* (for if/else) or a numeric
/// condition variable (for switch). Given the condition variable, you could obtain the SVFVar ID via
/// "edge->getCondition()->getId()" edge->getCondition() returns nullptr if this IntraCFGEdge is not a branch.
/// edge->getSuccessorCondValue() returns the actual condition value (1/0 for if/else) when this branch/IntraCFGEdge is
/// executed. For example, the successorCondValue is 1 on the edge from ICFGNode1 to ICFGNode2, and 0 on the edge from
/// ICFGNode1 to ICFGNode3
bool SSE::handleBranch(const IntraCFGEdge* edge) {
	/// TODO: your code starts from here

	// Get the condition variable (%cmp)
	const auto condVar = edge->getCondition();

	// Get the symbolic expression for the condition
	const auto condID = condVar->getId();
	const auto condExpr = getZ3Expr(condID);

	// Get the actual condition value (1/0 for if/else)
	const auto succCondVal = edge->getSuccessorCondValue();

	// Add the branch constraint: condition == succCondVal
	addToSolver(condExpr == getCtx().int_val((u32_t)succCondVal));

	// Check feasibility of the path under the added constraint
	if (getSolver().check() == unsat) {
		return false;
	}
	return true; // feasible
}

/// TODO: Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt and CmpStmt
/// Translate AddrStmt, CopyStmt, LoadStmt, StoreStmt, GepStmt, BinaryOPStmt, CmpStmt, SelectStmt, and PhiStmt
bool SSE::handleNonBranch(const IntraCFGEdge* edge) {
	const ICFGNode* dstNode = edge->getDstNode();
	const ICFGNode* srcNode = edge->getSrcNode();
	DBOP(if (!SVFUtil::isa<CallICFGNode>(dstNode) && !SVFUtil::isa<RetICFGNode>(dstNode)) std::cout
	     << "\n## Analyzing " << dstNode->toString() << "\n");

	for (const SVFStmt* stmt : dstNode->getSVFStmts()) {
		if (const AddrStmt* addr = SVFUtil::dyn_cast<AddrStmt>(stmt)) {
			/// TODO: implement AddrStmt handler here

			// x = &y
			u32_t lhsID = addr->getLHSVarID();
			u32_t rhsID = addr->getRHSVarID(); // y

			// x = get address of y
			expr addrExpr = getMemObjAddress(rhsID);
			z3Mgr->updateZ3Expr(lhsID, addrExpr);
		}
		else if (const CopyStmt* copy = SVFUtil::dyn_cast<CopyStmt>(stmt)) {
			/// TODO: implement CopyStmt handler her

			// x = y
			u32_t lhsID = copy->getLHSVarID();
			u32_t rhsID = copy->getRHSVarID();

			expr rhsExpr = getZ3Expr(rhsID);
			z3Mgr->updateZ3Expr(lhsID, rhsExpr);
		}
		else if (const LoadStmt* load = SVFUtil::dyn_cast<LoadStmt>(stmt)) {
			/// TODO: implement LoadStmt handler here

			// x = *p
			u32_t lhsID = load->getLHSVarID();
			u32_t ptrID = load->getRHSVarID(); // pointer to value

			expr ptrExpr = getZ3Expr(ptrID);
			expr valExpr = z3Mgr->loadValue(ptrExpr);
			z3Mgr->updateZ3Expr(lhsID, valExpr);
		}
		else if (const StoreStmt* store = SVFUtil::dyn_cast<StoreStmt>(stmt)) {
			/// TODO: implement StoreStmt handler here

			// *p = x
			u32_t ptrID = store->getLHSVarID();
			u32_t valID = store->getRHSVarID();

			expr ptrExpr = getZ3Expr(ptrID);
			expr valExpr = getZ3Expr(valID);
			z3Mgr->storeValue(ptrExpr, valExpr);
		}
		else if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
			/// TODO: implement GepStmt handler here

			u32_t lhsID = gep->getLHSVarID();
			u32_t baseID = gep->getRHSVarID();

			expr baseExpr = getZ3Expr(baseID);
			u32_t offset = z3Mgr->getGepOffset(gep, callingCtx);
			expr gepAddr = z3Mgr->getGepObjAddress(baseExpr, offset);
			z3Mgr->updateZ3Expr(lhsID, gepAddr);
		}
		/// Given a CmpStmt "r = a > b"
		/// cmp->getOpVarID(0)/cmp->getOpVarID(1) returns the first/second operand, i.e., "a" and "b"
		/// cmp->getResID() returns the result operand "r" and cmp->getPredicate() gives you the predicate ">"
		/// Find the comparison predicates in "class CmpStmt:Predicate" under SVF/svf/include/SVFIR/SVFStatements.h
		/// You are only required to handle integer predicates, including ICMP_EQ, ICMP_NE, ICMP_UGT, ICMP_UGE,
		/// ICMP_ULT, ICMP_ULE, ICMP_SGT, ICMP_SGE, ICMP_SLE, ICMP_SLT We assume integer-overflow-free in this
		/// assignment
		else if (const CmpStmt* cmp = SVFUtil::dyn_cast<CmpStmt>(stmt)) {
			/// TODO: implement CmpStmt handler here

			// r = a CMP b
			u32_t lhsID = cmp->getResID();
			expr op0 = getZ3Expr(cmp->getOpVarID(0));
			expr op1 = getZ3Expr(cmp->getOpVarID(1));

			expr result = getCtx().int_val(0); // default false

			switch (cmp->getPredicate()) {
			case CmpStmt::ICMP_EQ: result = ite(op0 == op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_NE: result = ite(op0 != op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_UGT: result = ite(ugt(op0, op1), getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_UGE: result = ite(uge(op0, op1), getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_ULT: result = ite(ult(op0, op1), getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_ULE: result = ite(ule(op0, op1), getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_SGT: result = ite(op0 > op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_SGE: result = ite(op0 >= op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_SLT: result = ite(op0 < op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			case CmpStmt::ICMP_SLE: result = ite(op0 <= op1, getCtx().int_val(1), getCtx().int_val(0)); break;
			default: assert(false && "Unsupported predicate in CmpStmt");
			}

			z3Mgr->updateZ3Expr(lhsID, result);
		}
		else if (const BinaryOPStmt* binary = SVFUtil::dyn_cast<BinaryOPStmt>(stmt)) {
			expr op0 = getZ3Expr(binary->getOpVarID(0));
			expr op1 = getZ3Expr(binary->getOpVarID(1));
			expr res = getZ3Expr(binary->getResID());
			switch (binary->getOpcode()) {
			case BinaryOperator::Add: addToSolver(res == op0 + op1); break;
			case BinaryOperator::Sub: addToSolver(res == op0 - op1); break;
			case BinaryOperator::Mul: addToSolver(res == op0 * op1); break;
			case BinaryOperator::SDiv: addToSolver(res == op0 / op1); break;
			case BinaryOperator::SRem: addToSolver(res == op0 % op1); break;
			case BinaryOperator::Xor: addToSolver(res == bv2int(int2bv(32, op0) ^ int2bv(32, op1), 1)); break;
			case BinaryOperator::And: addToSolver(res == bv2int(int2bv(32, op0) & int2bv(32, op1), 1)); break;
			case BinaryOperator::Or: addToSolver(res == bv2int(int2bv(32, op0) | int2bv(32, op1), 1)); break;
			case BinaryOperator::AShr: addToSolver(res == bv2int(ashr(int2bv(32, op0), int2bv(32, op1)), 1)); break;
			case BinaryOperator::Shl: addToSolver(res == bv2int(shl(int2bv(32, op0), int2bv(32, op1)), 1)); break;
			default: assert(false && "implement this part");
			}
		}
		else if (const BranchStmt* br = SVFUtil::dyn_cast<BranchStmt>(stmt)) {
			DBOP(std::cout << "\t skip handled when traversal Conditional IntraCFGEdge \n");
		}
		else if (const SelectStmt* select = SVFUtil::dyn_cast<SelectStmt>(stmt)) {
			expr res = getZ3Expr(select->getResID());
			expr tval = getZ3Expr(select->getTrueValue()->getId());
			expr fval = getZ3Expr(select->getFalseValue()->getId());
			expr cond = getZ3Expr(select->getCondition()->getId());
			addToSolver(res == ite(cond == getCtx().int_val(1), tval, fval));
		}
		else if (const PhiStmt* phi = SVFUtil::dyn_cast<PhiStmt>(stmt)) {
			expr res = getZ3Expr(phi->getResID());
			bool opINodeFound = false;
			for (u32_t i = 0; i < phi->getOpVarNum(); i++) {
				assert(srcNode && "we don't have a predecessor ICFGNode?");
				if (srcNode->getFun()->postDominate(srcNode->getBB(), phi->getOpICFGNode(i)->getBB())) {
					expr ope = getZ3Expr(phi->getOpVar(i)->getId());
					addToSolver(res == ope);
					opINodeFound = true;
				}
			}
			assert(opINodeFound && "predecessor ICFGNode of this PhiStmt not found?");
		}
	}

	return true;
}

/// Traverse each program path
bool SSE::translatePath(std::vector<const ICFGEdge*>& path) {
	for (const ICFGEdge* edge : path) {
		if (const IntraCFGEdge* intraEdge = SVFUtil::dyn_cast<IntraCFGEdge>(edge)) {
			if (handleIntra(intraEdge) == false)
				return false;
		}
		else if (const CallCFGEdge* call = SVFUtil::dyn_cast<CallCFGEdge>(edge)) {
			handleCall(call);
		}
		else if (const RetCFGEdge* ret = SVFUtil::dyn_cast<RetCFGEdge>(edge)) {
			handleRet(ret);
		}
		else
			assert(false && "what other edges we have?");
	}

	return true;
}

/// Program entry
void SSE::analyse() {
	for (const ICFGNode* src : identifySources()) {
		assert(SVFUtil::isa<GlobalICFGNode>(src) && "reachability should start with GlobalICFGNode!");
		for (const ICFGNode* sink : identifySinks()) {
			const IntraCFGEdge startEdge(nullptr, const_cast<ICFGNode*>(src));
			/// start traversing from the entry to each assertion and translate each path
			reachability(&startEdge, sink);
			resetSolver();
		}
	}
}

//===- Assignment-3.cpp -- Abstract Interpretation --//
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
 * Abstract Interpretation and buffer overflow detection
 *
 * Created on: Feb 19, 2024
 */

#include "Assignment_3.h"
#include "Util/Options.h"
#include "Util/WorkList.h"

using namespace SVF;
using namespace SVFUtil;

/// TODO : Implement the state updates for Copy, Binary, Store, Load, Gep, Phi
void AbstractExecution::updateStateOnCopy(const CopyStmt* copy) {
	/// TODO: your code starts from here

	const ICFGNode* node = copy->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);

	NodeID lhs = copy->getLHSVarID();
	NodeID rhs = copy->getRHSVarID();

	as[lhs] = as[rhs];
	// std::cout << std::dec;
	// std::cout << "after copy from " << rhs << " to " << lhs << "\n";
	// as.printAbstractState();
}

/// Find the comparison predicates in "class BinaryOPStmt:OpCode" under SVF/svf/include/SVFIR/SVFStatements.h
/// You are required to handle predicates (The program is assumed to have signed ints and also interger-overflow-free),
/// including Add, FAdd, Sub, FSub, Mul, FMul, SDiv, FDiv, UDiv, SRem, FRem, URem, Xor, And, Or, AShr, Shl, LShr
void AbstractExecution::updateStateOnBinary(const BinaryOPStmt* binary) {
	/// TODO: your code starts from here

	const ICFGNode* node = binary->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);

	NodeID lhs = binary->getResID();
	NodeID op1 = binary->getOpVarID(0);
	NodeID op2 = binary->getOpVarID(1);

	const IntervalValue& v1 = as[op1].getInterval();
	const IntervalValue& v2 = as[op2].getInterval();
	IntervalValue result;

	auto opCode = binary->getOpcode();
	switch (opCode) {
	case BinaryOPStmt::Add: result = v1 + v2; break;
	case BinaryOPStmt::Sub: result = v1 - v2; break;
	case BinaryOPStmt::Mul: result = v1 * v2; break;
	case BinaryOPStmt::SDiv: result = v1 / v2; break;
	case BinaryOPStmt::SRem: result = v1 % v2; break;
	case BinaryOPStmt::And: result = v1 & v2; break;
	case BinaryOPStmt::Or: result = v1 | v2; break;
	case BinaryOPStmt::Xor: result = v1 ^ v2; break;
	case BinaryOPStmt::Shl: result = v1 << v2; break;
	case BinaryOPStmt::AShr: result = v1 >> v2; break;
	default: result = IntervalValue::top(); break;
	}
	as[lhs] = result;
	// std::cout << std::dec;
	// std::cout << "after BinOp of " << opCode << " between VarID=" << op1 << " and VarID=" << op2 << " to VarID=" <<
	// lhs
	//           << "\n";
	// as.printAbstractState();
}

void AbstractExecution::updateStateOnStore(const StoreStmt* store) {
	/// TODO: your code starts from here

	const ICFGNode* node = store->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);

	auto ptrID = store->getLHSVarID();
	auto valID = store->getRHSVarID();

	auto addrVal = as[ptrID].getAddrs();
	auto valueToStore = as[valID];

	// std::cout << std::dec;
	// std::cout << "[Store] before store from VarID=" << valID << " to VarID=" << ptrID << "\n";
	// as.printAbstractState();

	for (u32_t rawAddr : addrVal.getVals()) {
		// Map the raw virtual address back to your internal addrID
		u32_t addrID = as.getIDFromAddr(rawAddr);
		// Actually perform the store into your abstract memory at addrID
		as.storeValue(addrID, valueToStore);
	}

	// std::cout << std::dec;
	// std::cout << "[Store] after store from VarID=" << valID << " to VarID=" << ptrID << "\n";
	// as.printAbstractState();
}

void AbstractExecution::updateStateOnLoad(const LoadStmt* load) {
	/// TODO: your code starts from here

	const auto* node = load->getICFGNode();
	auto& as = getAbsStateFromTrace(node);

	auto ptrID = load->getRHSVarID();
	auto lhsID = load->getLHSVarID();

	as[lhsID] = as.loadValue(ptrID);
	// std::cout << std::dec;
	// std::cout << "[Load] after load from " << ptrID << " to " << lhsID << "\n";
	// as.printAbstractState();
}

void AbstractExecution::updateStateOnGep(const GepStmt* gep) {
	/// TODO: your code starts from here

	const ICFGNode* node = gep->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);

	NodeID lhs = gep->getLHSVarID();
	NodeID rhs = gep->getRHSVarID();

	IntervalValue index = as.getElementIndex(gep);
	AddressValue gepAddrs = as.getGepObjAddrs(rhs, index);
	as[lhs] = AbstractValue(gepAddrs);
	std::cout << "[Gep] called\n";
}

void AbstractExecution::updateStateOnPhi(const PhiStmt* phi) {
	/// TODO: your code starts from here

	// Locate the ICFG node where the phi lives,
	// and get & modify its abstract state.
	const ICFGNode* phiNode = phi->getICFGNode();
	AbstractState& curState = getAbsStateFromTrace(phiNode);

	// std::cout << "[Phi] updateStateOnPhi: processing PhiStmt at node " << phiNode << std::endl;

	// Prepare to collect and join all incoming values.
	NodeID lhs = phi->getResID();
	u32_t numOps = phi->getOpVarNum();
	AbstractValue merged;

	// For each incoming edge i:
	for (u32_t i = 0; i < numOps; ++i) {
		// a) Find the predecessor node for operand i
		const ICFGNode* predNode = phi->getOpICFGNode(i);
		// b) Get that node's abstract state snapshot
		AbstractState& predState = getAbsStateFromTrace(predNode);
		// c) Extract the operand's value
		NodeID varID = phi->getOpVarID(i);
		AbstractValue value = predState[varID];

		// std::cout << "[Phi]  operand " << i << " from node " << predNode << " (varID=" << varID
		//           << "): " << value.toString() << std::endl;

		if (i == 0) {
			merged = value;
		}
		else {
			merged.join_with(value);
			// std::cout << "[Phi]  merged after operand " << i << ": " << merged.toString() << std::endl;
		}
	}

	// Assign the joined result to the phi-node's state
	curState[lhs] = merged;
	// std::cout << "[Phi]  final merged value assigned to varID=" << lhs << ": " << merged.toString() << std::endl;
}

/// TODO: handle GepStmt `lhs = rhs + off` and detect buffer overflow
// Step 1: For each `obj \in pts(rhs)`, get the size of allocated baseobj (entire mem object) via `obj_size =
// svfir->getBaseObj(objId)->getByteSizeOfObj();` There is a buffer overflow if `accessOffset.ub() >= obj_size`, where
// accessOffset is obtained via `getAccessOffset` Step 2: invoke `reportBufOverflow` with the current ICFGNode if an
// overflow is detected
void AbstractExecution::bufOverflowDetection(const SVF::SVFStmt* stmt) {
	if (!SVFUtil::isa<CallICFGNode>(stmt->getICFGNode())) {
		if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
			AbstractState& as = getAbsStateFromTrace(gep->getICFGNode());
			NodeID lhs = gep->getLHSVarID();
			NodeID rhs = gep->getRHSVarID();
			updateGepObjOffsetFromBase(as, as[lhs].getAddrs(), as[rhs].getAddrs(), as.getByteOffset(gep));

			/// TODO: your code starts from here

			// Step 6: Iterate over all memory objects that 'rhs' points to
			for (const auto& objAddr : as[rhs].getAddrs()) {
				// Step 7-8: Get the internal SVF variable ID for this address
				SVF::NodeID objId = as.getIDFromAddr(objAddr);

				// Step 9: Retrieve the base memory object and its byte size
				const auto baseObj = svfir->getBaseObject(objId);
				u32_t objSize = baseObj->getByteSizeOfObj();

				// Step 10: Calculate the access byte offset for this object and GEP
				IntervalValue accessOffset = as.getByteOffset(gep);
				auto upper = accessOffset.ub();
				// Step 11: Check if the upper bound of accessOffset exceeds or equals the object size
				if (upper.getIntNumeral() >= objSize) {
					// Step 12: Report potential buffer overflow at this ICFG node
					reportBufOverflow(gep->getICFGNode());
				}
			}
		}
	}
}

/**
 * @brief Handle ICFG nodes in a cycle using widening and narrowing operators
 *
 * This function implements abstract interpretation for cycles in the ICFG using widening and narrowing
 * operators to ensure termination. It processes all ICFG nodes within a cycle and implements
 * widening-narrowing iteration to reach fixed points twice: once for widening (to ensure termination)
 * and once for narrowing (to improve precision).
 *
 * @param cycle The WTO cycle containing ICFG nodes to be processed
 * @return void
 */
void AbstractExecution::handleICFGCycle(const ICFGCycleWTO* cycle) {
	// Get execution states from in edges
	bool is_feasible =
	    mergeStatesFromPredecessors(cycle->head()->getICFGNode(), preAbsTrace[cycle->head()->getICFGNode()]);
	if (!is_feasible)
		return;

	/// TODO: your code starts here

	const ICFGNode* l = cycle->head()->getICFGNode(); // ℓ := cycle.getHead().getICFGNode()
	bool increasing = true;
	int i = 0;

	std::cout << "[Cycle] Entering cycle at node " << l << std::endl;
	while (true) {
		AbstractState as_pre = getAbsStateFromTrace(l); // aspre := σℓ
		std::cout << "[Cycle] Iteration " << i << ", pre-state for node " << l->getId() << ":\n";
		// as_pre.printAbstractState();

		handleICFGNode(l); // handleICFGNode(ℓ)

		AbstractState as_cur = getAbsStateFromTrace(l);
		; // ascur := σℓ
		std::cout << "[Cycle] Iteration " << i << ", post-node state for node " << l->getId() << ":\n";
		// as_cur.printAbstractState();

		if (i >= Options::WidenDelay()) {
			if (increasing) {
				std::cout << "[Cycle] Widening at iteration " << i << std::endl;
				AbstractState widened = as_pre.widening(as_cur); // σℓ := aspre ▽ ascur
				std::cout << "[Cycle] Widen result for node " << l->getId() << ":\n";
				// widened.printAbstractState();
				preAbsTrace[l] = widened;

				if (widened.equals(as_pre)) { // σℓ ≡ aspre
					increasing = false; // increasing := false
					std::cout << "[Cycle] Widening stabilized; switching to narrowing" << std::endl;
					continue; // restart loop with narrowing
				}
			}
			else {
				std::cout << "[Cycle] Narrowing at iteration " << i << std::endl;
				AbstractState narrowed = as_pre.narrowing(as_cur); // σℓ := aspre ∆ ascur
				std::cout << "[Cycle] Narrow result for node " << l->getId() << ":\n";
				// narrowed.printAbstractState();
				preAbsTrace[l] = narrowed;

				if (narrowed.equals(as_pre)) { // σℓ ≡ aspre
					std::cout << "[Cycle] Narrowing reached fixed point; exiting cycle" << std::endl;
					break; // fixed-point reached
				}
			}
		}

		// Analyze remaining cycle components (WTO children)
		for (const ICFGWTOComp* comp : cycle->getWTOComponents()) {
			if (auto* singleton = dyn_cast<ICFGSingletonWTO>(comp)) {
				std::cout << "[Cycle] Handling singleton node " << singleton->getICFGNode()->getId() << std::endl;
				handleICFGNode(singleton->getICFGNode());
			}
			else if (auto* subCycle = dyn_cast<ICFGCycleWTO>(comp)) {
				std::cout << "[Cycle] Recursing into sub-cycle at node " << subCycle->head()->getICFGNode()->getId()
				          << std::endl;
				handleICFGCycle(subCycle);
			}
		}
		++i;
	}

	// std::cout << "[Cycle] Exiting cycle at node " << l->getId() << std::endl;
	return;
}

/// Abstract state updates on an AddrStmt
void AbstractExecution::updateStateOnAddr(const AddrStmt* addr) {
	const ICFGNode* node = addr->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	as.initObjVar(SVFUtil::cast<ObjVar>(addr->getRHSVar()));
	as[addr->getLHSVarID()] = as[addr->getRHSVarID()];
	// as.printAbstractState();
}

/// Abstract state updates on an CmpStmt
void AbstractExecution::updateStateOnCmp(const CmpStmt* cmp) {
	const ICFGNode* node = cmp->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	u32_t op0 = cmp->getOpVarID(0);
	u32_t op1 = cmp->getOpVarID(1);
	u32_t res = cmp->getResID();
	if (as.inVarToValTable(op0) && as.inVarToValTable(op1)) {
		IntervalValue resVal;
		IntervalValue &lhs = as[op0].getInterval(), &rhs = as[op1].getInterval();
		// AbstractValue
		auto predicate = cmp->getPredicate();
		switch (predicate) {
		case CmpStmt::ICMP_EQ:
		case CmpStmt::FCMP_OEQ:
		case CmpStmt::FCMP_UEQ: resVal = (lhs == rhs); break;
		case CmpStmt::ICMP_NE:
		case CmpStmt::FCMP_ONE:
		case CmpStmt::FCMP_UNE: resVal = (lhs != rhs); break;
		case CmpStmt::ICMP_UGT:
		case CmpStmt::ICMP_SGT:
		case CmpStmt::FCMP_OGT:
		case CmpStmt::FCMP_UGT: resVal = (lhs > rhs); break;
		case CmpStmt::ICMP_UGE:
		case CmpStmt::ICMP_SGE:
		case CmpStmt::FCMP_OGE:
		case CmpStmt::FCMP_UGE: resVal = (lhs >= rhs); break;
		case CmpStmt::ICMP_ULT:
		case CmpStmt::ICMP_SLT:
		case CmpStmt::FCMP_OLT:
		case CmpStmt::FCMP_ULT: resVal = (lhs < rhs); break;
		case CmpStmt::ICMP_ULE:
		case CmpStmt::ICMP_SLE:
		case CmpStmt::FCMP_OLE:
		case CmpStmt::FCMP_ULE: resVal = (lhs <= rhs); break;
		case CmpStmt::FCMP_FALSE: resVal = IntervalValue(0, 0); break;
		case CmpStmt::FCMP_TRUE: resVal = IntervalValue(1, 1); break;
		default: {
			assert(false && "undefined compare: ");
		}
		}
		as[res] = resVal;
	}
	else if (as.inVarToAddrsTable(op0) && as.inVarToAddrsTable(op1)) {
		IntervalValue resVal;
		AbstractValue &lhs = as[op0], &rhs = as[op1];
		auto predicate = cmp->getPredicate();
		switch (predicate) {
		case CmpStmt::ICMP_EQ:
		case CmpStmt::FCMP_OEQ:
		case CmpStmt::FCMP_UEQ: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(lhs.equals(rhs));
			}
			else {
				if (lhs.getAddrs().hasIntersect(rhs.getAddrs())) {
					resVal = IntervalValue::top();
				}
				else {
					resVal = IntervalValue(0);
				}
			}
			break;
		}
		case CmpStmt::ICMP_NE:
		case CmpStmt::FCMP_ONE:
		case CmpStmt::FCMP_UNE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(!lhs.equals(rhs));
			}
			else {
				if (lhs.getAddrs().hasIntersect(rhs.getAddrs())) {
					resVal = IntervalValue::top();
				}
				else {
					resVal = IntervalValue(1);
				}
			}
			break;
		}
		case CmpStmt::ICMP_UGT:
		case CmpStmt::ICMP_SGT:
		case CmpStmt::FCMP_OGT:
		case CmpStmt::FCMP_UGT: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() > *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_UGE:
		case CmpStmt::ICMP_SGE:
		case CmpStmt::FCMP_OGE:
		case CmpStmt::FCMP_UGE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() >= *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_ULT:
		case CmpStmt::ICMP_SLT:
		case CmpStmt::FCMP_OLT:
		case CmpStmt::FCMP_ULT: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() < *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::ICMP_ULE:
		case CmpStmt::ICMP_SLE:
		case CmpStmt::FCMP_OLE:
		case CmpStmt::FCMP_ULE: {
			if (lhs.getAddrs().size() == 1 && rhs.getAddrs().size() == 1) {
				resVal = IntervalValue(*lhs.getAddrs().begin() <= *rhs.getAddrs().begin());
			}
			else {
				resVal = IntervalValue::top();
			}
			break;
		}
		case CmpStmt::FCMP_FALSE: resVal = IntervalValue(0, 0); break;
		case CmpStmt::FCMP_TRUE: resVal = IntervalValue(1, 1); break;
		default: {
			assert(false && "undefined compare: ");
		}
		}
		as[res] = resVal;
	}
}

/// Abstract state updates on an CallPE
void AbstractExecution::updateStateOnCall(const CallPE* call) {
	const ICFGNode* node = call->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	NodeID lhs = call->getLHSVarID();
	NodeID rhs = call->getRHSVarID();
	as[lhs] = as[rhs];
}

/// Abstract state updates on an RetPE
void AbstractExecution::updateStateOnRet(const RetPE* retPE) {
	const ICFGNode* node = retPE->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	NodeID lhs = retPE->getLHSVarID();
	NodeID rhs = retPE->getRHSVarID();

	// std::cout << "\n[Return] Processing return at ICFGNode: " << node->getId() << "\n";
	// std::cout << "[Return] LHS Var" << lhs << " ← RHS Var" << rhs << "\n";
	// std::cout << "before return:";
	// as.printAbstractState();
	as[lhs] = as[rhs];
	// std::cout << "after return:";
	// as.printAbstractState();
}

/// Abstract state updates on an SelectStmt
void AbstractExecution::updateStateOnSelect(const SelectStmt* select) {
	const ICFGNode* node = select->getICFGNode();
	AbstractState& as = getAbsStateFromTrace(node);
	u32_t res = select->getResID();
	u32_t tval = select->getTrueValue()->getId();
	u32_t fval = select->getFalseValue()->getId();
	u32_t cond = select->getCondition()->getId();
	if (as[cond].getInterval().is_numeral()) {
		as[res] = as[cond].getInterval().is_zero() ? as[fval] : as[tval];
	}
	else {
		as[res].join_with(as[tval]);
		as[res].join_with(as[fval]);
	}
}

void AbstractExecution::updateStateOnExtCall(const CallICFGNode* extCallNode) {
	std::string funcName = extCallNode->getCalledFunction()->getName();

	if (funcName == "mem_insert") {
		AbstractState& as = getAbsStateFromTrace(extCallNode);

		// Arguments: mem_insert(dst_buffer, offset, length)
		NodeID bufferId = extCallNode->getArgument(0)->getId();
		NodeID offsetId = extCallNode->getArgument(1)->getId();
		NodeID lenId = extCallNode->getArgument(2)->getId();

		const AbstractValue& bufferVal = as.loadValue(bufferId);
		const AbstractValue& offsetVal = as.loadValue(offsetId);
		const AbstractValue& lenVal = as.loadValue(lenId);

		IntervalValue offsetIv = offsetVal.getInterval();
		IntervalValue lenIv = lenVal.getInterval();

		IntervalValue accessRange = offsetIv + lenIv;

		for (u32_t addr : bufferVal.getAddrs()) { // Use getAddrs() from AddressValue
			NodeID objID = as.getIDFromAddr(addr);
			const BaseObjVar* baseObj = svfir->getBaseObject(objID); // Adjust method name if needed
			u32_t objSize = baseObj->getByteSizeOfObj();

			if (accessRange.ub().getIntNumeral() > objSize) {
				reportBufOverflow(extCallNode);
			}
			else {
				// Pass ValVar* or NodeID as per utils->handleMemcpy definition
				utils->handleMemcpy(as,
				                    extCallNode->getArgument(0), // dst buffer ValVar*
				                    extCallNode->getArgument(1), // src offset ValVar*, may need src pointer instead
				                    as[lenId].getInterval(), // data size ValVar*
				                    offsetVal.getInterval().getIntNumeral()); // start position ValVar*
			}
		}
	}
	else if (funcName == "str_insert") {
		AbstractState& as = getAbsStateFromTrace(extCallNode);

		NodeID bufferID = extCallNode->getArgument(0)->getId();
		NodeID strID = extCallNode->getArgument(1)->getId();
		NodeID positionID = extCallNode->getArgument(2)->getId();

		// Use utils helper to get string length (returns AbstractValue)
		AbstractValue dataSizeVal = utils->getStrlen(as, extCallNode->getArgument(1));

		AbstractValue bufferAddrs = as.loadValue(bufferID);
		AbstractValue positionVal = as.loadValue(positionID);

		IntervalValue offsetInt = positionVal.getInterval() + dataSizeVal.getInterval();

		for (u32_t addr : bufferAddrs.getAddrs()) {
			NodeID objId = as.getIDFromAddr(addr);
			const BaseObjVar* baseObj = svfir->getBaseObject(objId);
			u32_t objSize = baseObj->getByteSizeOfObj();

			if (offsetInt.ub().getIntNumeral() > objSize) {
				reportBufOverflow(extCallNode);
			}
			else {
				utils->handleMemcpy(as,
				                    extCallNode->getArgument(0), // dst buffer ValVar*
				                    extCallNode->getArgument(1), // src string ValVar*
				                    dataSizeVal.getInterval(), // length AbstractValue (from getStrlen)
				                    positionVal.getInterval().getIntNumeral() // start position ValVar*
				);
			}
		}
	}
}
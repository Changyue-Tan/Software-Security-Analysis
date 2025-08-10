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
	// std::cout << "[Gep] called\n";
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
// Step 1:
//			For each `obj \in pts(rhs)`, get the size of allocated baseobj (entire mem object) via
// 			`obj_size = svfir->getBaseObj(objId)->getByteSizeOfObj();`
// 			There is a buffer overflow if `accessOffset.ub() >= obj_size`,
// 			where accessOffset is obtained via `getAccessOffset`
// Step 2:
//			invoke `reportBufOverflow` with the current ICFGNode if an overflow is detected
void AbstractExecution::bufOverflowDetection(const SVF::SVFStmt* stmt) {
	if (!SVFUtil::isa<CallICFGNode>(stmt->getICFGNode())) {
		if (const GepStmt* gep = SVFUtil::dyn_cast<GepStmt>(stmt)) {
			AbstractState& as = getAbsStateFromTrace(gep->getICFGNode());
			NodeID lhs = gep->getLHSVarID();
			NodeID rhs = gep->getRHSVarID();
			updateGepObjOffsetFromBase(as, as[lhs].getAddrs(), as[rhs].getAddrs(), as.getByteOffset(gep));

			/// TODO: your code starts from here

			IntervalValue accessOffset = as.getByteOffset(gep);
			u64_t accessSize = 1; // conservative assumption
			bool reportOnUnknownOffset = true;

			for (const auto& objAddr : as[rhs].getAddrs()) {
				NodeID objId = as.getIDFromAddr(objAddr);
				auto* baseObj = svfir->getBaseObject(objId); // correct API in SVF
				if (!baseObj)
					continue;

				u64_t objSize = baseObj->getByteSizeOfObj();

				if (accessOffset.isBottom()) {
					// no reachable offset
					continue;
				}

				if (accessOffset.isTop()) {
					if (reportOnUnknownOffset) {
						reportBufOverflow(gep->getICFGNode());
					}
					continue;
				}

				auto ubVal = accessOffset.ub();
				if (ubVal.is_plus_infinity()) {
					if (reportOnUnknownOffset) {
						reportBufOverflow(gep->getICFGNode());
					}
					continue;
				}

				long long ubLL = ubVal.getIntNumeral();
				if (ubLL < 0) {
					// negative offset → potential underflow
					reportBufOverflow(gep->getICFGNode());
					continue;
				}

				u64_t accessUpperByte = static_cast<u64_t>(ubLL) + (accessSize - 1);
				if (accessUpperByte >= objSize) {
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

	const ICFGNode* l = cycle->head()->getICFGNode();
	bool increasing = true;
	int i = 0;

	while (true) {
		AbstractState as_pre = getAbsStateFromTrace(l);

		handleICFGNode(l);

		AbstractState as_cur = getAbsStateFromTrace(l);

		if (i >= Options::WidenDelay()) {
			if (increasing) {
				// widening: sigma_l := as_pre ▽ as_cur
				AbstractState widened = as_pre.widening(as_cur);
				preAbsTrace[l] = widened;

				if (widened.equals(as_pre)) {
					increasing = false;
					continue;
				}
			}
			else {
				AbstractState narrowed = as_pre.narrowing(as_cur);
				preAbsTrace[l] = narrowed;

				if (narrowed.equals(as_pre)) {
					break;
				}
			}
		}

		// analyze remaining WTO components after fixed points
		for (const ICFGWTOComp* comp : cycle->getWTOComponents()) {
			if (auto* singleton = dyn_cast<ICFGSingletonWTO>(comp)) {
				handleICFGNode(singleton->getICFGNode());
			}
			else if (auto* subCycle = dyn_cast<ICFGCycleWTO>(comp)) {
				handleICFGCycle(subCycle);
			}
		}

		i++;
	}

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
	AbstractState& as = getAbsStateFromTrace(extCallNode);

	// as.printAbstractState();

	// std::cerr << "[Info] updateStateOnExtCall called for function: " << funcName << "\n";

	if (funcName == "mem_insert") {
		NodeID bufferId = extCallNode->getArgument(0)->getId();
		NodeID dataId = extCallNode->getArgument(1)->getId();
		NodeID dataSizeId = extCallNode->getArgument(2)->getId();
		NodeID positionId = extCallNode->getArgument(3)->getId();

		// std::cerr << "[Debug] mem_insert args NodeIDs: buffer=" << bufferId << ", data=" << dataId
		//           << ", dataSize=" << dataSizeId << ", position=" << positionId << "\n";

		// get buffer address set
		const AbstractValue& bufferAddrs = as[bufferId];
		// get data size value
		const AbstractValue& dataSizeVal = as[dataSizeId];
		const AbstractValue& positionVal = as[positionId];

		// std::cerr << "[Debug] bufferAddrs: " << bufferAddrs.toString() << "\n";
		// std::cerr << "[Debug] dataSizeVal: " << dataSizeVal.toString() << "\n";
		// std::cerr << "[Debug] positionVal: " << positionVal.toString() << "\n";

		IntervalValue offsetIv = positionVal.getInterval();
		IntervalValue sizeIv = dataSizeVal.getInterval();

		// if (offsetIv.isBottom()) {
		// 	std::cerr << "[Error] position interval is bottom (invalid)!\n";
		// }
		// if (sizeIv.isBottom()) {
		// 	std::cerr << "[Error] dataSize interval is bottom (invalid)!\n";
		// }

		IntervalValue accessOffset = offsetIv + sizeIv;

		// if (accessOffset.isBottom()) {
		// 	std::cerr << "[Error] accessOffset interval is bottom after addition!\n";
		// }
		// else {
		// 	std::cerr << "[Debug] accessOffset interval after addition: " << accessOffset.toString() << "\n";
		// }

		for (u32_t addr : bufferAddrs.getAddrs()) {
			NodeID objID = as.getIDFromAddr(addr);
			const BaseObjVar* baseObj = svfir->getBaseObject(objID);
			u32_t objSize = baseObj->getByteSizeOfObj();

			// std::cerr << "[Trace] Checking object ID=" << objID << " size=" << objSize << " bytes\n";

			if (!accessOffset.isBottom() && accessOffset.ub().getIntNumeral() >= objSize) {
				// std::cerr << "[Alert] Buffer overflow detected!\n";
				reportBufOverflow(extCallNode);
			}
			else {
				utils->handleMemcpy(as,
				                    extCallNode->getArgument(0), // dst buffer
				                    extCallNode->getArgument(1), // src data
				                    sizeIv,
				                    offsetIv.ub().getIntNumeral());
			}
		}
	}
	else if (funcName == "str_insert") {
		NodeID bufferId = extCallNode->getArgument(0)->getId();
		NodeID strArgId = extCallNode->getArgument(1)->getId();
		NodeID positionId = extCallNode->getArgument(2)->getId();

		// std::cerr << "[Debug] str_insert args NodeIDs: buffer=" << bufferId << ", str=" << strArgId
		//           << ", position=" << positionId << "\n";

		const AbstractValue& bufferAddrs = as[bufferId];
		// std::cerr << "[Debug] bufferAddrs: " << bufferAddrs.toString() << "\n";

		const AbstractValue& positionVal = as[positionId];
		AbstractValue dataSizeVal = utils->getStrlen(as, extCallNode->getArgument(1));

		// std::cerr << "[Debug] positionVal: " << positionVal.toString() << "\n";
		// std::cerr << "[Debug] dataSizeVal (strlen): " << dataSizeVal.toString() << "\n";

		IntervalValue posIv = positionVal.getInterval();
		IntervalValue sizeIv = dataSizeVal.getInterval();
		IntervalValue accessOffset = posIv + sizeIv;

		if (posIv.isBottom()) {
			// std::cerr << "[Error] position interval is bottom (invalid)!\n";
			return;
		}
		if (sizeIv.isBottom()) {
			// std::cerr << "[Error] dataSize interval is bottom (invalid)!\n";
			return;
		}
		if (accessOffset.isBottom()) {
			// std::cerr << "[Error] accessOffset interval is bottom after addition!\n";
			return;
		}

		// std::cerr << "[Debug] accessOffset interval: " << accessOffset.toString() << "\n";

		for (u32_t addr : bufferAddrs.getAddrs()) {
			NodeID objID = as.getIDFromAddr(addr);
			const BaseObjVar* baseObj = svfir->getBaseObject(objID);
			u32_t objSize = baseObj->getByteSizeOfObj();

			// std::cerr << "[Trace] Checking buffer object ID: " << objID << " with size: " << objSize << "\n";

			if (accessOffset.ub().getIntNumeral() >= objSize) {
				// std::cerr << "[Warning] Buffer overflow detected! accessOffset.ub = "
				//           << accessOffset.ub().getIntNumeral() << " >= objSize = " << objSize << "\n";
				reportBufOverflow(extCallNode);
			}
			else {
				// std::cerr << "[Info] No overflow. Proceeding with handleMemcpy.\n";
				utils->handleMemcpy(as,
				                    extCallNode->getArgument(0), // dst buffer
				                    extCallNode->getArgument(1), // src string
				                    sizeIv,
				                    posIv.ub().getIntNumeral());
			}
		}
	}
}
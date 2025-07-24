//===- AEMgr.cpp -- Abstract Execution Manual Translations---------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
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

//
// Created on 2024/1/10.
//

#include "AEMgr.h"

namespace SVF {

	AEState AbstractExecutionMgr::test0() {
		/*
		// A simple example

		        int main() {
		            int* p;
		            int q;
		            int* r;
		            int x;

		            p = malloc();
		            q = 5;
		            *p = q;
		            x = *p;
		            assert(x==5);
		        }
		*/

		AEState as;
		//  int** p;
		NodeID p = getNodeID("p");

		//  int q;
		NodeID q = getNodeID("q");

		//  int* r;
		NodeID r = getNodeID("r");

		//  int x;
		NodeID x = getNodeID("x");

		// p = malloc(..);
		NodeID malloc = getNodeID("malloc");
		as[p] = AddressValue(getMemObjAddress("malloc"));

		// q = 5;
		as[q] = IntervalValue(5, 5);

		// *p = q;
		as.storeValue(p, as[q]);

		// x = *p;
		as[x] = as.loadValue(p);
		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test1() {
		//    int main() {
		//        int a;
		//        int b;
		//        a = 0;
		//        b = a + 1;
		//        assert(b>0);
		//    }
		AEState as;
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		as[a] = IntervalValue(0, 0);
		as[b] = as[a].getInterval() + IntervalValue(1, 1);
		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test2() {
		//    int main() {
		//        int* p;
		//        int q;
		//        int b;
		//
		//        p = malloc;
		//        *p = 0;
		//        q = *p;
		//        *p = 3;
		//        b = *p + 1;
		//        assert(b>3);
		//    }
		AEState as;
		NodeID p = getNodeID("p");
		NodeID q = getNodeID("q");
		NodeID b = getNodeID("b");
		/// TODO: your code starts from here

		// p = malloc
		NodeID malloc = getNodeID("malloc");
		as[p] = AddressValue(getMemObjAddress("malloc"));

		// *p = 0;
		as.storeValue(p, IntervalValue(0, 0));

		// q = *p;
		as[q] = as.loadValue(p);

		// *p = 3;
		as.storeValue(p, IntervalValue(3, 3));

		// b = *p + 1;
		as[b] = as.loadValue(p).getInterval() + IntervalValue(1, 1);

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test3() {
		//    int main() {
		//        int** p;
		//        int* q;
		//        int* r;
		//        int x;
		//
		//        p = malloc1(..);
		//        q = malloc2(..);
		//        *p = q;
		//        *q = 10;
		//        r = *p;
		//        x = *r;
		//        assert(x==10);
		//    }
		AEState as;
		NodeID p = getNodeID("p");
		NodeID q = getNodeID("q");
		NodeID r = getNodeID("r");
		NodeID x = getNodeID("x");
		/// TODO: your code starts from here

		// p = malloc1();
		NodeID malloc1 = getNodeID("malloc1");
		as[p] = AddressValue(getMemObjAddress("malloc1"));

		// q = malloc2();
		NodeID malloc2 = getNodeID("malloc2");
		as[q] = AddressValue(getMemObjAddress("malloc2"));

		// *p = q;
		as.storeValue(p, as[q]);

		// *q = 10;
		as.storeValue(q, IntervalValue(10, 10));

		// r = *p;
		as[r] = as.loadValue(p);

		// x = *r;
		as[x] = as.loadValue(r);

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test4() {
		//    int main() {
		//        int* p;
		//        int* x;
		//        int* y;
		//        int a;
		//        int b;
		//        p = malloc;
		//        x = &p[0];
		//        y = &p[1]
		//        *x = 10;
		//        *y = 11;
		//        a = *x;
		//        b = *y;
		//        assert((a + b)>20);
		AEState as;
		NodeID p = getNodeID("p");
		NodeID x = getNodeID("x");
		NodeID y = getNodeID("y");
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		/// TODO: your code starts from here

		// p = malloc;
		NodeID malloc = getNodeID("malloc");
		as[p] = AddressValue(getMemObjAddress("malloc"));

		// x = &p[0];
		as[x] = AddressValue(getGepObjAddress("p", 0));

		// y = &p[1];
		as[y] = AddressValue(getGepObjAddress("p", 1));

		// *x = 10;
		as.storeValue(x, IntervalValue(10, 10));

		// *y = 11;
		as.storeValue(y, IntervalValue(11, 11));

		// a = *x;
		as[a] = as.loadValue(x);

		// b = *y;
		as[b] = as.loadValue(y);

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test5() {
		//// Struct and pointers
		//
		//    struct A{ int f0; int* f1;};
		//    int main() {
		//       struct A* p;
		//       int* x;
		//       int* q;
		//       int** r;
		//       int* y;
		//       int z;
		//
		//       p = malloc1;
		//       x = malloc2;
		//       *x = 5;
		//       q = &(p->f0);
		//       *q = 10;
		//       r = &(p->f1);
		//       *r = x;
		//       y = *r;
		//       z = *q + *y;
		//       assert(z==15);
		AEState as;
		NodeID p = getNodeID("p", 2);
		NodeID x = getNodeID("x");
		NodeID q = getNodeID("q");
		NodeID r = getNodeID("r");
		NodeID y = getNodeID("y");
		NodeID z = getNodeID("z");
		/// TODO: your code starts from here

		// Step 1: p = malloc1;
		NodeID malloc1 = getNodeID("malloc1");
		as[p] = AddressValue(getMemObjAddress("malloc1"));

		// Step 2: x = malloc2;
		NodeID malloc2 = getNodeID("malloc2");
		as[x] = AddressValue(getMemObjAddress("malloc2"));

		// Step 3: *x = 5;
		as.storeValue(x, IntervalValue(5, 5));

		// Step 4: q = &(p->f0);
		as[q] = AddressValue(getGepObjAddress("p", 0));

		// Step 5: *q = 10;
		as.storeValue(q, IntervalValue(10, 10));

		// Step 6: r = &(p->f1);
		as[r] = AddressValue(getGepObjAddress("p", 1));

		// Step 7: *r = x;
		as.storeValue(r, as[x]);

		// Step 8: y = *r;
		as[y] = as.loadValue(r); // dereferencing r to get x

		// Step 9: z = *q + *y;
		auto val_q = as.loadValue(q).getInterval();
		auto val_y = as.loadValue(y).getInterval();
		as[z] = val_q + val_y;

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test6() {
		//    int main(int argv) {  // argv is an interval  [4, 10]
		//    int a;
		//    int b;
		//    a = argv + 1;
		//    b = 5;
		//    if(a > 10)
		//       b = a;
		//    assert(b>=5);
		//    }
		AEState as;
		NodeID a = getNodeID("a");
		NodeID b = getNodeID("b");
		NodeID argv = getNodeID("argv");
		/// TODO: your code starts from here

		// argv is in [4, 10]
		as[argv] = IntervalValue(4, 10);

		// a = argv + 1 => [5, 11]
		as[a] = as[argv].getInterval() + IntervalValue(1, 1);

		// b = 5 => [5, 5]
		as[b] = IntervalValue(5, 5);

		AEState as_after_if;

		// Feasibility of condition (a > 10)
		AbstractValue cmp_true = as[a].getInterval() > IntervalValue(10, 10);
		cmp_true.meet_with(IntervalValue(1, 1)); // check if condition can be true
		if (!cmp_true.getInterval().isBottom()) {
			AEState then_branch = as;
			// Restrict a to [11, +INF]
			then_branch[a].meet_with(IntervalValue(11, IntervalValue::plus_infinity()));
			// b = a in this branch
			then_branch[b] = then_branch[a];
			as_after_if.joinWith(then_branch);
		}

		// Feasibility of else branch (a <= 10)
		AbstractValue cmp_false = as[a].getInterval() > IntervalValue(10, 10);
		cmp_false.meet_with(IntervalValue(0, 0)); // check if condition can be false
		if (!cmp_false.getInterval().isBottom()) {
			AEState else_branch = as;
			// Restrict a to [-INF, 10]
			else_branch[a].meet_with(IntervalValue(IntervalValue::minus_infinity(), 10));
			// b is not changed
			as_after_if.joinWith(else_branch);
		}

		as = as_after_if;

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test7() {
		// int foo(int z) {
		//     k = z;
		//     return k;
		// }
		// int main() {
		//   int x;
		//   int y;
		//   y = foo(2);
		//   x = foo(3);
		//   assert(x== 3 && y==2);
		// }

		AEState as;
		NodeID x = getNodeID("x");
		NodeID y = getNodeID("y");
		/// TODO: your code starts from here

		// First call: foo(2)
		{
			AEState foo_call;
			NodeID k = getNodeID("k");
			NodeID z = getNodeID("z");

			// z = 2
			foo_call[z] = IntervalValue(2, 2);

			// k = z
			foo_call[k] = foo_call[z];

			// return k
			as[y] = foo_call[k];
		}

		// Second call: foo(3)
		{
			AEState foo_call;
			NodeID k = getNodeID("k");
			NodeID z = getNodeID("z");

			// z = 3
			foo_call[z] = IntervalValue(3, 3);

			// k = z
			foo_call[k] = foo_call[z];

			// return k
			as[x] = foo_call[k];
		}

		as.printAbstractState();
		return as;
	}

	AEState AbstractExecutionMgr::test8() {
		// int main() {
		//    int x;
		//    x=20;
		//    while(x>0) {
		//        x--;
		//    }
		//    assert(x == 0);
		//    return 0;
		// }


		//+---------+
		//| entry   |
		//| int x;  |
		//| x = 20  |
		//+---+-----+
		//    |
		//    v
		//+-----------+
		//|   head    | <-------+
		//|   x>0?    |         |
		//+---+------++         |
		//    |      |          |
		//  (x<=0)  (x>0)       |
		//    v      v          |
		//+-------+  +------+   |
		//| exit  |  | body |---|
		//|       |  |  x-- |
		//+---+---+  +------+



		AEState entry_as;
		AEState cur_head_as;
		AEState body_as;
		AEState exit_as;
		u32_t widen_delay = 3;
		NodeID x = getNodeID("x");
		/// TODO: your code starts from here

		// Compose 'entry_as' (x = 20)
		entry_as[x] = IntervalValue(20, 20);
		bool increasing = true;

		for (int cur_iter = 0;; ++cur_iter) {
			if (cur_iter >= widen_delay) {
				// Handle widening and narrowing after widen_delay
				AEState prev_head_as = cur_head_as;
				// Update head's state by joining with 'entry_as' and 'body_as'
				cur_head_as = entry_as;
				cur_head_as.joinWith(body_as);

				if (increasing) {
					// Widening phase
					AEState after_widen = prev_head_as.widening(cur_head_as);
					cur_head_as = after_widen;
					if (cur_head_as == prev_head_as) {
						// Fixpoint reached, start narrowing
						increasing = false;
						continue;
					}
				}
				else {
					// Narrow phase after widening
					AEState after_narrow = prev_head_as.narrowing(cur_head_as);
					cur_head_as = after_narrow;
					if (cur_head_as == prev_head_as) {
						// Fixpoint reached, exit loop
						break;
					}
				}
			}
			else {
				// Handle widen_delay, update cycle head's state
				// via joining entry_as and body_as
				cur_head_as = entry_as;
				cur_head_as.joinWith(body_as);
			}

			// Handle loop body by propagating head's state
			// meet with loop condition and enter loop body;
			body_as = cur_head_as;
			body_as[x].meet_with(IntervalValue(1, IntervalValue::plus_infinity()));
			body_as[x] = body_as[x].getInterval() - IntervalValue(1, 1);
		}

		// Propagate head_as to loop exit
		exit_as = cur_head_as;
		// Process loop exit condition (x <= 0)
		exit_as[x].meet_with(IntervalValue(IntervalValue::minus_infinity(), 0));

		exit_as.printAbstractState();
		return exit_as;
	}

} // namespace SVF
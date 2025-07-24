//===- Z3Examples.cpp -- Manual assertion-based verification (Z3 Example) ------------------//
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
 * Manual assertion-based verification (Z3 Example)
 *
 * Created on: Feb 19, 2024
 */

#include "Z3Examples.h"
using namespace z3;
using namespace SVF;

/* A simple example

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

void Z3Examples::test0() {
	//  int* p;
	expr p = getZ3Expr("p");

	//  int q;
	expr q = getZ3Expr("q");

	//  int* r;
	expr r = getZ3Expr("r");

	//  int x;
	expr x = getZ3Expr("x");

	//  p = malloc();
	addToSolver(p == getMemObjAddress("malloc"));

	//  q = 5;
	addToSolver(q == 5);

	//  *p = q;
	storeValue(p, q);

	//  x = *p;
	addToSolver(x == loadValue(p));
}

/*
// Simple integers

    int main() {
        int a;
        int b;
        a = 0;
        b = a + 1;
        assert(b > 0);
    }
*/
void Z3Examples::test1() {
	// int a;
	expr a = getZ3Expr("a");
	// int b;
	expr b = getZ3Expr("b");
	/// TODO: your code starts from here

	// a = 0;
	addToSolver(a == getZ3Expr(0));

	// b = a + 1;
	addToSolver(b == a + 1);
}
/*
  // One-level pointers

    int main() {
        int* p;
        int q;
        int b;
        p = malloc;
        *p = 0;
        q = *p;
        *p = 3;
        b = *p + 1;
        assert(b > 3);
    }
*/
void Z3Examples::test2() {
	// int *p;
	expr p = getZ3Expr("p");
	// int q;
	expr q = getZ3Expr("q");
	// int b;
	expr b = getZ3Expr("b");
	/// TODO: your code starts from here

    // Allocate memory and assign to p
    expr malloc1 = getMemObjAddress("malloc1");
    addToSolver(p == malloc1);

    // *p = 0;
    storeValue(p, getZ3Expr(0));

    // q = *p;
    addToSolver(q == loadValue(p));

    // *p = 3;
    storeValue(p, getZ3Expr(3));

    // b = *p + 1;
    addToSolver(b == loadValue(p) + 1);
}

/*
    // Mutiple-level pointers

    int main() {
        int** p;
        int* q;
        int* r;
        int x;

        p = malloc1(..);
        q = malloc2(..);
        *p = q;
        *q = 10;
        r = *p;
        x = *r;
        assert(x==10);
    }
*/
void Z3Examples::test3() {
	//  int** p;
	expr p = getZ3Expr("p");

	//  int* q;
	expr q = getZ3Expr("q");

	//  int* r;
	expr r = getZ3Expr("r");

	//  int x;
	expr x = getZ3Expr("x");

	/// TODO: your code starts from here

    // p = malloc1(..);
    expr malloc1 = getMemObjAddress("malloc1");
    addToSolver(p == malloc1);

    // q = malloc2(..);
    expr malloc2 = getMemObjAddress("malloc2");
    addToSolver(q == malloc2);

    // *p = q;
    storeValue(p, q);  

    // *q = 10;
    storeValue(q, getZ3Expr(10));  

    // r = *p;
    addToSolver(r == loadValue(p)); 

    // x = *r;
    addToSolver(x == loadValue(r));  
}

/*
   // Array and pointers

    int main() {
        int* p;
        int* x;
        int* y;
        int a;
        int b;
        p = malloc;
        x = &p[0];
        y = &p[1]
        *x = 10;
        *y = 11;
        a = *x;
        b = *y;
        assert((a + b)>20);
    }
*/
void Z3Examples::test4() {
	//  int* p;
	expr p = getZ3Expr("p");

	//  int* x;
	expr x = getZ3Expr("x");

	//  int* y;
	expr y = getZ3Expr("y");

	//  int a;
	expr a = getZ3Expr("a");

	//  int b;
	expr b = getZ3Expr("b");

	/// TODO: your code starts from here

    // p = malloc;
    expr malloc1 = getMemObjAddress("malloc1");
    addToSolver(p == malloc1);

    // x = &p[0];
    addToSolver(x == getGepObjAddress(p, 0));

    // y = &p[1];
    addToSolver(y == getGepObjAddress(p, 1));

    // *x = 10;
    storeValue(x, getZ3Expr(10));

    // *y = 11;
    storeValue(y, getZ3Expr(11));

    // a = *x;
    addToSolver(a == loadValue(x));

    // b = *y;
    addToSolver(b == loadValue(y));
}

/*
    // Branches

int main(int argv) {
    int a;
    int b;
    int b1;
    a = argv + 1;
    b = 5;
    if(a > 10)
        b = a;
    b1 = b;
    assert(b1 >= 5);
}
*/
void Z3Examples::test5() {
	// int argv
	expr argv = getZ3Expr("argv");

	//  int a;
	expr a = getZ3Expr("a");

	//  int b;
	expr b = getZ3Expr("b");

	//  int b1;
	expr b1 = getZ3Expr("b1");
	/// TODO: your code starts from here

    // a = argv + 1;
    addToSolver(a == argv + 1);

    /*
    b = 5;
    if(a > 10)
        b = a;
    b1 = b;
    */
    addToSolver(b == ite(a > 10, a, getZ3Expr(5)));

    // b1 = b;
    addToSolver(b1 == b);
}

/*
// Compare and pointers
int main() {
   int *a = malloc1;
   int *b = malloc2;
   *a = 5;
   *b = 10;
   int *p;
   if (*a < *b) {
       p = a;
   } else {
       p = b;
   }
   assert(*p == 5);
}
*/
void Z3Examples::test6() {
	//  int *a;
	expr a = getZ3Expr("a");
	//  int *b;
	expr b = getZ3Expr("b");
	/// TODO: your code starts from here

    // memory allocation for a and b
    expr malloc1 = getMemObjAddress("malloc1");
    expr malloc2 = getMemObjAddress("malloc2");
    addToSolver(a == malloc1);
    addToSolver(b == malloc2);

    // *a = 5;
    storeValue(a, getZ3Expr(5));

    // *b = 10;
    storeValue(b, getZ3Expr(10));

    // int *p;
    expr p = getZ3Expr("p");

    /*
    if (*a < *b) {
       p = a;
    } else {
       p = b;
    }
    */
    expr cond = loadValue(a) < loadValue(b); // *a < *b
    addToSolver(p == ite(cond, a, b));       // p = (cond ? a : b)
}

/*
 int main() {
    int a = 1, b = 2, c = 3;
    int d;
  if (a > 0) {
    d = b + c;
  }
  else {
    d = b - c;
  }
  assert(d == 5);
 }
 */
void Z3Examples::test7() {
	//  int a = 1, b = 2, c = 3;
	expr a = getZ3Expr("a");
	expr b = getZ3Expr("b");
	expr c = getZ3Expr("c");
	//  int d;
	expr d = getZ3Expr("d");
	/// TODO: your code starts from here

    // int a = 1, b = 2, c = 3;
    addToSolver(a == getZ3Expr(1));
    addToSolver(b == getZ3Expr(2));
    addToSolver(c == getZ3Expr(3));

    /*
    int d;
    if (a > 0) {
        d = b + c;
    } else {
        d = b - c;
    }
    */
    expr cond = a > getZ3Expr(0);
    addToSolver(d == ite(cond, b + c, b - c));
}

/*
 int main() {
    int arr[2] = {0, 1};
    int a = 10;
    int *p;
    if (a > 5) {
        p = &arr[0];
    }
    else {
        p = &arr[1];
    }
    assert(*p == 0);
 }
 */
void Z3Examples::test8() {
	//  int arr[2];
	expr arr = getZ3Expr("arr");
	//  int *p
	expr p = getZ3Expr("p");
	/// TODO: your code starts from here

    // arr = {0, 1} 
    addToSolver(arr == getMemObjAddress("malloc"));
    expr arr_0 = getGepObjAddress(arr, 0);
    expr arr_1 = getGepObjAddress(arr, 1);
    storeValue(arr_0, getZ3Expr(0));  // arr[0] = 0
    storeValue(arr_1, getZ3Expr(1));  // arr[1] = 1

    // int a = 10;
    expr a = getZ3Expr("a");

    // int a = 10
    addToSolver(a == getZ3Expr(10));

    /*
    if (a > 5) {
        p = &arr[0];
    }
    else {
        p = &arr[1];
    }
    */
    expr cond = a > getZ3Expr(5);
    addToSolver(p == ite(cond, arr_0, arr_1));
}

/*
    // Struct and pointers

    struct A{ int f0; int* f1;};
    int main() {
       struct A* p;
       int* x;
       int* q;
       int** r;
       int* y;
       int z;

       p = malloc1;
       x = malloc2;
       *x = 5;
       q = &(p->f0);
       *q = 10;
       r = &(p->f1);
       *r = x;
       y = *r;
       z = *q + *y;
       assert(z == 15);
    }
*/
void Z3Examples::test9() {
	// struct A* p;
	expr p = getZ3Expr("p");

	// int* x;
	expr x = getZ3Expr("x");

	// int* q;
	expr q = getZ3Expr("q");

	// int** r;
	expr r = getZ3Expr("r");

	// int* y;
	expr y = getZ3Expr("y");

	// int z;
	expr z = getZ3Expr("z");
	/// TODO: your code starts from here

    // memory allocation for p and x
	expr malloc1 = getMemObjAddress("malloc1"); // struct A
	expr malloc2 = getMemObjAddress("malloc2"); // int*

	addToSolver(p == malloc1);
	addToSolver(x == malloc2);

	// *x = 5;
	storeValue(x, getZ3Expr(5));

	// q = &(p->f0);
	expr p_f0 = getGepObjAddress(p, 0); // A->first
	addToSolver(q == p_f0);

	// *q = 10;
	storeValue(q, getZ3Expr(10));

	// r = &(p->f1);
	expr p_f1 = getGepObjAddress(p, 1); // A->second
	addToSolver(r == p_f1);

	// *r = x;
	storeValue(r, x);

	// y = *r;
	addToSolver(y == loadValue(r));

	// z = *q + *y;
	addToSolver(z == loadValue(q) + loadValue(y));
}

/*
int foo(int z) {
    k = z;
    return k;
}
int main() {
  int x;
  int y;
  y = foo(2);
  x = foo(3);
  assert(x == 3 && y == 2);
}
*/
void Z3Examples::test10() {
	// int x;
	expr x = getZ3Expr("x");

	// int y;
	expr y = getZ3Expr("y");

	// int z;
	expr z = getZ3Expr("z");

	// int k;
	expr k = getZ3Expr("k");
	/// TODO: your code starts from here

   	// First call: y = foo(2)
	solver.push();
	addToSolver(z == getZ3Expr(2));
	addToSolver(k == z);
	expr ret1 = getEvalExpr(k);
	solver.pop();
	addToSolver(y == ret1);

	// Second call: x = foo(3)
	solver.push();
	addToSolver(z == getZ3Expr(3));
	addToSolver(k == z);
	expr ret2 = getEvalExpr(k);
	solver.pop();
	addToSolver(x == ret2);
}
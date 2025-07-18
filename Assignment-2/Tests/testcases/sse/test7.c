#include <stdbool.h>
extern void svf_assert(bool);

// interprocedural store: writes 99 into *p
void foo(int* p) {
    *p = 99;
}

int main() {
    // 1) Local stack object, direct Addr + Store + Load
    int a = 0;
    int* pa = &a;      // AddrStmt for local a → should call createExprForObjVar/getMemObjAddress
    *pa = 42;          // StoreStmt → storeValue at local‑a address
    svf_assert(a == 42);

    // 2) Interprocedural case: local b passed into foo()
    int b = 0;
    foo(&b);           // handleCall must bind formal p → &b; handleRet pops context
    svf_assert(b == 99);

    return 0;
}

#include "stdbool.h"
//ADDR, LOAD, STORE, CAST(COPY)
extern void svf_assert(bool);

void foo(int* p) {
    *p = 4321;
}

int main() {
    int a = 1234;
    foo(&a);
    svf_assert(a == 4321);
}
#include <stdbool.h>
extern void svf_assert(bool);

int main() {
    int a = 0;
    int* p = &a;   // store address into pointer
    *p = 42;       // store value at pointer
    int b = *p;    // load value from pointer

    svf_assert(b == 42);  // check if load was correct
    return 0;
}

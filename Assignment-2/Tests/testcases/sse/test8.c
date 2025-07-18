#include <stdbool.h>
extern void svf_assert(bool);

// Stores v1 into *p and v2 into *q
void set_two(int* p, int* q, int v1, int v2) {
    *p = v1;
    *q = v2;
}

// Loads *p and *q, computes sum and product, writes back via out pointers
void compute(int* p, int* q, int* out_sum, int* out_prod) {
    int a = *p;
    int b = *q;
    *out_sum  = a + b;
    *out_prod = a * b;
}

int main() {
    // Test 1: multiple stores in one call
    int x = 0, y = 0;
    set_two(&x, &y, 7, 11);
    svf_assert(x == 7);
    svf_assert(y == 11);

    // Test 2: multiple loads + stores in one call
    int s = 0, p = 0;
    x = 3; y = 5;
    compute(&x, &y, &s, &p);
    // s = x + y = 8; p = x * y = 15
    svf_assert(s == 8);
    svf_assert(p == 15);

    return 0;
}

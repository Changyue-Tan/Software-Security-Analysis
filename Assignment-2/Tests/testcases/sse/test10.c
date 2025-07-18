#include <stdbool.h>
extern void svf_assert(bool);

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d;

    int *p = &a;
    int *q = &b;

    *p = *p + 4;    // a = 5
    *q = *q * 3;    // b = 6

    int *r = p;
    *r = *r + *q;   // a = 5 + 6 = 11

    c = a + b;      // c = 11 + 6 = 17
    d = c - 1;      // d = 16

    svf_assert(a == 11);
    svf_assert(b == 6);
    svf_assert(c == 17);
    svf_assert(d == 16);
    svf_assert(*p == 11);
    svf_assert(*r == a);
    svf_assert(p == r);

    return 0;
}

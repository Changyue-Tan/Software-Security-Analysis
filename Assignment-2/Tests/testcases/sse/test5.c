#include <stdbool.h>
#include <stdint.h>
extern void svf_assert(bool);

// 1) Struct with nested array
typedef struct {
    int a;
    int b[2];
} S;

// 2) Function pointer type
typedef int (*BinOp)(int, int);

// 3) Simple binary operations
int add(int x, int y)   { return x + y; }
int sub(int x, int y)   { return x - y; }
int mul(int x, int y)   { return x * y; }
int bitand_fn(int x, int y) { return x & y; }
int bitxor_fn(int x, int y) { return x ^ y; }
int shl_fn(int x, int y)    { return x << y; }
int shr_fn(int x, int y)    { return x >> y; }

// 4) Recursive factorial (one-level recursion)
int fact(int n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);
}

// 5) Copy memory with pointer casts
void copy4bytes(void* dst, void* src) {
    uint32_t* d = (uint32_t*)dst;
    uint32_t* s = (uint32_t*)src;
    *d = *s;
}

// 6) Swap half-bytes via bitwise
void swap_nibbles(uint8_t* p) {
    uint8_t v = *p;
    uint8_t hi = (v & 0xF0) >> 4;
    uint8_t lo = (v & 0x0F) << 4;
    *p = hi | lo;
}

int main() {
    // —— Local Addr / Copy / Load / Store —— 
    int x = 42;
    svf_assert(x == 42);

    int* px = &x;
    *px = x + 1;
    svf_assert(x == 43);

    // —— All binary ops —— 
    int y = 10, z = 3;
    svf_assert((y - z) == sub(y,z));
    svf_assert((y * z) == mul(y,z));
    svf_assert((y / z) == (y / z));
    svf_assert((y % z) == (y % z));
    svf_assert((y & z) == bitand_fn(y,z));
    svf_assert((y ^ z) == bitxor_fn(y,z));
    svf_assert((y << 1) == shl_fn(y,1));
    svf_assert((y >> 1) == shr_fn(y,1));

    // —— Compare predicates —— 
    svf_assert(y == 10);
    svf_assert(y != z);
    svf_assert(y > z);
    svf_assert(y >= z);
    svf_assert(z < y);
    svf_assert(z <= y);

    // —— Recursive factorial one‑step —— 
    svf_assert(fact(4) == 24);

    // —— Struct & GEP —— 
    S s = { .a = 5, .b = { 1, 2 } };
    S* ps = &s;
    ps->a = ps->a + 10;
    svf_assert(s.a == 15);
    ps->b[1] = ps->b[0] * 3;
    svf_assert(s.b[1] == 3);

    // —— Array & pointer arithmetic —— 
    int arr[4] = { 0, 1, 2, 3 };
    int* pa = arr;
    *(pa + 2) = 20;
    svf_assert(arr[2] == 20);
    int v = pa[3];
    svf_assert(v == 3);

    // —— select (ternary) + φ via if/else —— 
    int c;
    if (s.a > s.b[1]) {
        c = s.a - s.b[1];
    } else {
        c = s.b[1] - s.a;
    }
    svf_assert(c == (s.a > s.b[1] ? s.a - s.b[1] : s.b[1] - s.a));

    // —— Function pointer call —— 
    BinOp fp = &add;
    int r = fp(7, 8);
    svf_assert(r == 15);

    return 0;
}

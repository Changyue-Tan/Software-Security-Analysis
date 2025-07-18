#include <stdbool.h>
extern void svf_assert(bool);

typedef struct { int x, y; } Point;

// A helper to swap via pointer args
void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Increment both fields of a Point via pointer
void bump(Point* p) {
    p->x = p->x + 1;
    p->y = p->y + 2;
}

int main() {
    // 1) Simple direct store & load
    int a = 5;
    int b = a;                // CopyStmt
    svf_assert(b == 5);

    // 2) Addr + store through pointer
    int *p = &a;              // AddrStmt
    *p = 7;                   // StoreStmt
    svf_assert(a == 7);

    // 3) Interprocedural swap
    int x = 3, y = 4;
    swap(&x, &y);
    svf_assert(x == 4);
    svf_assert(y == 3);

    // 4) Struct fields & GEP
    Point pt = { .x = 10, .y = 20 };
    bump(&pt);
    svf_assert(pt.x == 11);
    svf_assert(pt.y == 22);

    // 5) Array + GEP + load
    int arr[3] = { 2, 4, 6 };
    int *q = arr;             // &arr[0]
    int v1 = q[1];            // equivalent to *(&arr + 1)
    svf_assert(v1 == 4);
    int *r = &arr[2];
    svf_assert(r[0] == 6);

    // 6) Conditional (Select) and Phi via two paths
    int c;
    if (a > b) {
        c = a - b;
    } else {
        c = b - a;
    }
    // c = |a - b|
    svf_assert(c == (a > b ? a - b : b - a));

    // 7) Combined pointer arithmetic
    int z = 0;
    int idx = 2;
    int *base = &arr[0];
    int *elem = base + idx;  // GEP: ptr + offset
    *elem = 42;               // StoreStmt through computed address
    svf_assert(arr[2] == 42);

    return 0;
}

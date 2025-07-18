#include <stdbool.h>
extern void svf_assert(bool);

// selectAndStore: stores the larger of v1,v2 into *p, otherwise v2 into *q
void selectAndStore(int* p, int* q, int v1, int v2) {
    if (v1 > v2) {
        *p = v1;
    } else {
        *q = v2;
    }
}

int main() {
    int A = 0, B = 0;

    // Case 1: v1>v2 → *p = v1
    selectAndStore(&A, &B, 5, 3);
    svf_assert(A == 5);
    svf_assert(B == 0);



    return 0;
}

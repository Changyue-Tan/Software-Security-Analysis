#include <stdbool.h>

extern void svf_assert(bool);

// Interprocedural function with multi-parameter and pointer store
void update_sum(int* out, int a, int b) {
    *out = a + b;  // store
}

// Called multiple times with different parameters
int main() {
    int result1 = 0;
    int result2 = 0;
    int result3 = 0;

    update_sum(&result1, 1, 2);  // result1 = 3
    update_sum(&result2, 10, 5); // result2 = 15
    update_sum(&result3, 7, 8);  // result3 = 15

    // Load values and check correctness
    svf_assert(result1 == 3);
    svf_assert(result2 == 15);
    svf_assert(result3 == 15);

    return 0;
}

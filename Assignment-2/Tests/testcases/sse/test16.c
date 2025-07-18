#include <stdbool.h>
extern void svf_assert(bool);

// Innermost function: adds two numbers
int add(int a, int b) {
    return a + b;
}

// Mid-level function: adds three numbers by calling add twice
int add_three(int x, int y, int z) {
    int temp = add(x, y);
    return add(temp, z);
}

// Top-level function: calls add_three with given parameters
int process(int p, int q, int r) {
    return add_three(p, q, r);
}

int main() {
    int res1 = process(1, 2, 3);  // 1 + 2 + 3 = 6
    int res2 = process(4, 5, 6);  // 4 + 5 + 6 = 15

    svf_assert(res1 == 6);
    svf_assert(res2 == 15);

    return 0;
}

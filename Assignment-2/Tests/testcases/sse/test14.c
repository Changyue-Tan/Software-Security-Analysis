#include <stdbool.h>

extern void svf_assert(bool);

int add(int a, int b) {
    return a + b;
}

int main() {
    int x = add(1, 2);  // 3
    int y = add(3, 4);  // 7
    int z = add(5, 6);  // 11

    svf_assert(x == 3);
    svf_assert(y == 7);
    svf_assert(z == 11);

    return 0;
}

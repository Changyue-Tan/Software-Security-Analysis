#include <stdbool.h>
extern void svf_assert(bool);

int main() {
    int a = 20;
    int b = 6;

    // 1) Addition
    svf_assert((a + b) == 26);

    // 2) Subtraction
    svf_assert((a - b) == 14);

    // 3) Multiplication
    svf_assert((a * b) == 120);

    // 4) Signed division
    svf_assert((a / b) == 3);

    // 5) Signed remainder
    svf_assert((a % b) == 2);

    // 6) Bitwise AND
    svf_assert((a & b) == (20 & 6));

    // 7) Bitwise OR
    svf_assert((a | b) == (20 | 6));

    // 8) Bitwise XOR
    svf_assert((a ^ b) == (20 ^ 6));

    // 9) Logical left shift (Shl)
    svf_assert((a << 1) == 40);

    // 10) Arithmetic right shift (AShr)
    svf_assert((a >> 1) == 10);

    return 0;
}

#include <stdbool.h>
extern void svf_assert(bool);

// Innermost function: returns max of two numbers
int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

// Mid-level function: returns max of three numbers by calling max twice
int max_of_three(int x, int y, int z) {
    int temp = max(x, y);
    if (temp > z)
        return temp;
    else
        return z;
}

// Top-level function: calls max_of_three and branches based on the result
int process(int p, int q, int r) {
    int m = max_of_three(p, q, r);
    if (m > 10)
        return m - 10;
    else
        return 10 - m;
}

int main() {
    int res1 = process(3, 7, 5);   // max_of_three = 7 → 10 - 7 = 3
    int res2 = process(12, 8, 9);  // max_of_three = 12 → 12 - 10 = 2

    svf_assert(res1 == 3);
    svf_assert(res2 == 2);

    return 0;
}

#include <stdbool.h>
extern void svf_assert(bool);

int compute_sum(int x, int y) {
    int result;
    if (x > y) {
        result = x + y;
    } else {
        result = y - x;
    }
    return result;
}

int compute_product(int a, int b) {
    int result;
    if (a % 2 == 0) {
        result = a * b;
    } else {
        result = a + b * 2;
    }
    return result;
}

int process(int m, int n) {
    int sum = compute_sum(m, n);         // load m, n → call compute_sum → store to sum
    int prod = compute_product(m, n);    // load m, n → call compute_product → store to prod

    int final_result;
    if (sum < prod) {
        final_result = prod - sum;
    } else {
        final_result = sum - prod;
    }
    return final_result;                 // load final_result
}

int main() {
    int x = 3;
    int y = 5;

    int res1 = process(x, y);   // multiple interprocedural calls
    int res2 = process(6, 2);

    svf_assert(res1 == 10);     // (5 - 3) = 2, 3 + 5*2 = 13 → 13 - 2 = 11
    svf_assert(res2 == 4);      // (6 + 2) = 8, 6*2 = 12 → 12 - 8 = 4

    return 0;
}

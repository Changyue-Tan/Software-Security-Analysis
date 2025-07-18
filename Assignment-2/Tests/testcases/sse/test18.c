#include <stdbool.h>
extern void svf_assert(bool);

// Innermost function: increments the value pointed by ptr by the given amount
void increment(int* ptr, int amount) {
    int val = *ptr;        // load
    val = val + amount;    // arithmetic
    *ptr = val;            // store
}

// Mid-level function: calls increment twice with different amounts
void double_increment(int* ptr, int inc1, int inc2) {
    increment(ptr, inc1);
    increment(ptr, inc2);
}

// Top-level function: calls double_increment
void process(int* p, int a, int b) {
    double_increment(p, a, b);
}

int main() {
    int value = 5;
    process(&value, 3, 4);  // value should be incremented by 3 then by 4

    svf_assert(value == 12); // 5 + 3 + 4 = 12

    return 0;
}

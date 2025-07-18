#include <stdbool.h>
extern void svf_assert(bool);

// Innermost function: updates *ptr based on a condition (branch)
void update_value(int* ptr, int delta) {
    int val = *ptr;         // load
    if (val > 10) {         // branch
        val = val + delta;
    } else {
        val = val - delta;
    }
    *ptr = val;             // store
}

// Mid-level function: decides delta based on input, then calls update_value
void conditional_update(int* ptr, int x) {
    int delta;
    if (x % 2 == 0) {       // branch
        delta = 5;
    } else {
        delta = 3;
    }
    update_value(ptr, delta);
}

// Top-level function: calls conditional_update twice with nested calls
void process(int* p, int a, int b) {
    conditional_update(p, a);
    conditional_update(p, b);
}

int main() {
    int value = 12;
    process(&value, 4, 7);

    // Calculation:
    // First call: x=4 even => delta=5; val=12 > 10 => val=12+5=17
    // Second call: x=7 odd => delta=3; val=17 > 10 => val=17+3=20
    svf_assert(value == 20);

    return 0;
}

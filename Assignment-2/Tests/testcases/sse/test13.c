extern void svf_assert(int);

// Callee function: no load/store, just computation
int compute(int x) {
    if (x > 0) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    int res = compute(42);  // Interprocedural call

    svf_assert(res == 1);   // ✅ Assertion checks returned value
    return 0;
}

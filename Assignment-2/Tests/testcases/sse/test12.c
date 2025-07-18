extern void svf_assert(int);

int main() {
    int x = 5;
    int a = 0, b = 0;
    int* p;

    if (x > 0) {
        p = &a;
    } else {
        p = &b;
    }

    *p = 42;

    int y = *p;

    svf_assert(y == 42);  // ✅ assertion here
    return 0;
}

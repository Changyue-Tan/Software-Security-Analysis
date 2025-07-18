#include <stdbool.h>
extern void svf_assert(bool);

// Writes 42 into *p
void store42(int* p) {
    *p = 42;
}

// Reads and returns *p
int load_val(int* p) {
    return *p;
}

int main() {
    int x = 0;

    // Interprocedural store: store42(&x) must update x in main
    store42(&x);
    svf_assert(x == 42);

    // Interprocedural load: load_val(&x) must see that updated value
    int y = load_val(&x);
    svf_assert(y == 42);

    return 0;
}

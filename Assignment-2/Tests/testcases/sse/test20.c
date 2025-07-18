#include <stdbool.h>
extern void svf_assert(bool);

// Struct used for pointer arithmetic (GepStmt)
typedef struct {
    int a;
    int b;
    int c;
} Data;

// Function 1: copies input to output with a condition (CmpStmt + branch)
void copy_if_greater(int* out, int x, int y) {
    if (x > y) {               // CmpStmt + branch
        *out = x;              // StoreStmt
    } else {
        *out = y;              // StoreStmt
    }
}

// Function 2: updates fields of Data based on condition and nested call
void update_data(Data* d, int x, int y) {
    int temp;
    copy_if_greater(&temp, x, y);  // nested call with AddrStmt and StoreStmt
    d->a = temp;                   // StoreStmt (GepStmt + store)
    
    if (temp % 2 == 0) {           // branch + CmpStmt
        d->b = temp * 2;           // StoreStmt
    } else {
        d->b = temp + 1;           // StoreStmt
    }
    
    // LoadStmt (reading d->a and d->b)
    int sum = d->a + d->b;         // CopyStmt + BinaryOPStmt

    d->c = sum;                    // StoreStmt
}

// Top-level function: calls update_data multiple times (nested interprocedural calls with parameters)
void process_data(Data* data, int x, int y, int z) {
    update_data(data, x, y);
    update_data(data, y, z);
}

int main() {
    Data d = {0, 0, 0};
    process_data(&d, 3, 5, 2);

    // Breakdown:
    // call 1: copy_if_greater(&temp, 3, 5) => temp = 5
    // d.a = 5
    // temp % 2 == 1 -> else branch, d.b = 6
    // d.c = 5 + 6 = 11
    // call 2: copy_if_greater(&temp, 5, 2) => temp = 5
    // d.a = 5 (overwrite)
    // temp % 2 == 1 -> else branch, d.b = 6 (overwrite)
    // d.c = 5 + 6 = 11 (overwrite)

    svf_assert(d.a == 5);
    svf_assert(d.b == 6);
    svf_assert(d.c == 11);

    return 0;
}

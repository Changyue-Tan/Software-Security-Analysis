#include <stdbool.h>

void str_insert(void* buffer, const void* data, int position);
extern void OVERFLOW(void* data, int size);
extern void svf_assert(bool condition);

int main() {
    char buffer[15] = {0}; // Bigger buffer for mixed tests

    // 1️⃣ Insert at the start
    str_insert(buffer, "Hello", 0);
    svf_assert(buffer[0] == 'H');
    svf_assert(buffer[4] == 'o');

    // 2️⃣ Insert in the middle
    str_insert(buffer, "123", 5); // After "Hello"
    svf_assert(buffer[5] == '1');
    svf_assert(buffer[7] == '3');

    // 3️⃣ Insert near the end (safe)
    str_insert(buffer, "XY", 12);
    svf_assert(buffer[12] == 'X');
    svf_assert(buffer[13] == 'Y');

    // 4️⃣ Insert causing overflow
    str_insert(buffer, "ABCDEFG", 10); // This will exceed size 15
    OVERFLOW(buffer, 16); // Trigger detection

    return 0;
}

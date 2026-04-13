#include <stdint.h>
#include <stdio.h>

int main() {
    char string[100] = "";
    scanf("%s", string);

    uint64_t hash = (uint64_t) string[0];
    int idx = 0;

    constexpr uint64_t len = (sizeof(uint64_t) * 8) - 1;
    while (string[idx] != '\0') {
        hash ^= hash >> 33;
        hash = (hash << 1) | (hash >> len);
        hash *= 0xff51afd7ed588ccd;
        // hash = (hash >> 1) | (hash << len);
        hash ^= (uint64_t) string[idx++];
    }
    printf("%lu\n", hash);

    return 0;
}

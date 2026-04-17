#include <valgrind/callgrind.h>
#include <x86intrin.h>
#include "type.h"
#include "common.h"
#include "func.h"

constexpr int SIZE_TABLE = 18043;
// constexpr int SIZE_STRING = 32;
constexpr char DATA_FILE[] = "Data/2.txt";
constexpr char DUMP_FILE[] = "dump.txt";

int main() {
    KDS_HashMap map = {};
    KDS_TextContext cont = {};
    volatile KDS_HashMapList *list = NULL;

    KDS_HM_Create(&map, SIZE_TABLE);
    KDS_TC_Create(&cont, DATA_FILE);

    // printf("Step 1\n");
    KDS_HM_LoadData(&map, &cont);
    KDS_HM_DumpText(&map, DUMP_FILE);
    KDS_HM_DumpDat(&map, "dump2.dat");

    constexpr int AMOUNT = 1;

#ifdef TEST
    uint64_t data[AMOUNT] = {};
#endif /* TEST */

CALLGRIND_START_INSTRUMENTATION;

    for (int idx = 0; idx < AMOUNT; idx++) {
        KDS_TC_SetWord(&cont, 0);

#ifdef TEST
        uint64_t start = __rdtsc();
#endif /* TEST */
        for (int i = 0; i < cont.ptr.size; i++) {
            char *string = KDS_TC_GetNextWord(&cont);
            list = KDS_HM_FindString32(&map, string);
        }
#ifdef TEST
        uint64_t end = __rdtsc();
        data[idx] = end - start;
#endif /* TEST */
    }

CALLGRIND_STOP_INSTRUMENTATION;

#ifdef TEST

    uint64_t mid = 0;
    for (int i = 0; i < AMOUNT; i++) {
        mid += data[i] / (uint64_t) AMOUNT;
    }

    uint64_t dec = 0;
    for (int i = 0; i < AMOUNT; i++) {
        if (data[i] > mid) {
            dec += (data[i] - mid) * (data[i] - mid);
        } else {
            dec += (mid - data[i]) * (mid - data[i]);
        }
    }

    printf("MID: %lu\nDEC: %lu\n", mid, dec / (AMOUNT * AMOUNT));
#endif /* TEST */

    printf("%p\n", list);

    KDS_HM_Destroy(&map);
    KDS_TC_Destroy(&cont);

    return 0;
}

#include <stdio.h>
#include <math.h>

const char *NAME_FILE = "dump2.dat";

int main() {
    FILE *stream = fopen(NAME_FILE, "rb");

    int size = 0;
    int data[5000] = {};
    int sum = 0;

    int number = 0;
    int value = 0;

    while (true) {
        if (fscanf(stream, "%d %d", &number, &value) != 2) {
            break;
        }

        // printf("%d %d\n", number, value);
        if (value != 0) {
            sum += value;
            data[size++] = value;
        }
    }
    fclose(stream);

    // printf("SUM: %d\nSIZE: %d\n", sum, size);
    // sum /= size;
    float mid = (float) sum / (float) size;
    float dec = 0;

    for (int i = 0; i < size; i++) {
        // printf("%f\n", (float) data[i]);
        dec += ((float) data[i] - mid) * ((float) data[i] - mid);
    }
    dec /= (float) size;

    printf("MID: %f\nDEC: %g\n", mid, sqrt(dec));
    return 0;
}


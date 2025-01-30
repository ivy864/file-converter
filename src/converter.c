#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void write_header(FILE *out) {
    uint64_t mthd = 0x4d546864;
    fwrite(&mthd, 4, 1, out);

    uint64_t len = 6;
    fwrite(&len, 4, 1, out);

    uint16_t format = 0;
    fwrite(&len, 2, 1, out);

    uint16_t n = 1;
    fwrite(&n, 2, 1, out);

    int16_t delta = 100;
    fwrite(&delta, 2, 1, out);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: converter <in> <out>\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");

    if (in == NULL || out == NULL) {
        fprintf(stderr, "Invalid file!\n");
        return EXIT_FAILURE;
    }
    
    write_header(out);

    return EXIT_SUCCESS;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** 
 * Write a 32 bit integer to a file, in order of highest bytes to lowest.
 */
void write_int(FILE *file, uint64_t num, uint32_t size) {
    uint8_t byte;
    for (int i = size; i > 0; i--) {
        byte = num >> 8 * (i - 1);
        putc(byte, file);

        printf("byte: %x\n", byte);
    }

}

void write_header(FILE *out) {
    uint32_t mthd = 0x6468544d;
    fwrite(&mthd, 4, 1, out);

    uint32_t len = 0x06000000;
    fwrite(&len, 4, 1, out);

    uint16_t format = 0;
    fwrite(&format, 2, 1, out);

    uint16_t n = 0x0100;
    fwrite(&n, 2, 1, out);

    int16_t delta = 100;
    fwrite(&delta, 2, 1, out);
}

void write_track(FILE *out, FILE *in) {
    uint32_t mtrk = 0x6b72544d;
    fwrite(&mtrk, 1, 4, out);

    fseek(in, 0, SEEK_END);
    uint32_t size = ftell(in);
    fseek(in, 0, SEEK_SET);

    uint32_t extra = size % 3;
    size += extra;
    // notes are delta + (event code + channel) + note + attack 
    size = size + (size / 3);
    // all off = 4
    // end = 4

    fwrite(&size, 4, 1, out);

    unsigned char c; 

    uint32_t count = 0;
    while (fread(&c, 1, 1, in) != 0) {
        if (count % 3 == 0) {
            c  = c | 0x80;
        }
        fwrite(&c, 1, 1, out);
        count++;
    }
    uint32_t end = 	0x00FF2F00;
    fwrite(&end, 4, 1, out);
}

void write_test(FILE *out) {
    uint32_t mtrk = 0x6b72544d;
    fwrite(&mtrk, 1, 4, out);

    uint32_t size = 0x11000000;
    fwrite(&size, 4, 1, out);

    uint32_t on = 0x7f349001;
    uint32_t off = 0x7f34807f;
    uint32_t alloff = 0x7bb0109c;

    fwrite(&on, 4, 1, out);
    fwrite(&off, 4, 1, out);
    fwrite(&alloff, 4, 1, out);
    putc(0, out);
    // 6 bytes
    // 3 + 1 3 + 1
    // 8 bytes
    // 6 + (6/3)
    // 7 bytes -> extra delta
    // bytes += bytes % 3
    // (bytes = 9)
    // delta -> operation + channel -> 
    

    uint32_t end = 	0x002fff00;
    fwrite(&end, 4, 1, out);
}


int main(int argc, char *argv[]) {
    write_int(stdout, 0xffaabbcc, 2);
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
    write_test(out);
    //write_track(out, in);

    return EXIT_SUCCESS;
}

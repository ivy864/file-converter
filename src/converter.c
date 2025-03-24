#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** 
 * Write 'size' bytes of an integer (num) to a file, from highest order bytes to lowest.
 */
void write_int(FILE *file, uint64_t num, uint32_t size) {
    uint8_t byte;
    for (int i = size; i > 0; i--) {
        byte = num >> 8 * (i - 1);
        putc(byte, file);
    }
}

/**
 * Write a MIDI header for a format 0 MIDI file with 1 track.
 */
void write_header(FILE *out) {
    // write the string "MThd"
    write_int(out, 0x4d546864, 4);

    // size of header chunk 
    write_int(out, (uint64_t) 6, 4);

    // MIDI format. Format 0 is a single track MIDI file
    write_int(out, 0, 2);

    // Number of tracks. Since we're using format 0, this must be 1
    write_int(out, 1, 2);

    // Number of ticks per quarter-note
    write_int(out, 96, 2);
}

void write_track(FILE *out, FILE *in) {
    // write the string MTrk 
    write_int(out, 0x4d54726b, 4);

    /* 
     * Note on/off events are composed of 5 sections:
     * - time delta (variable length; at least 1 byte)
     * - event code (4 bits) and channel (4 bits)
     * - note (1 byte)
     * - attack (1 byte)
     *
     * Event code and channel need to be something valid, so they alternate 
     * between note-on (9) and note off (8).
     * Since code and channel do not come from the input file, they add an 
     * additional byte for every 3 bytes.
     */
    fseek(in, 0, SEEK_END);
    uint32_t size = ftell(in);
    fseek(in, 0, SEEK_SET);

    uint32_t extra = 3 - (size % 3);
    size += extra;

    size = size + (size / 3);

    // add 8 bytes for the 'all notes off' and 'end of track' messages
    size += 8;

    write_int(out, size, 4);

    uint8_t c; 
    uint32_t count = 0;

    while (fread(&c, 1, 1, in) != 0) {
        // clear highest order bit
        c  = c & 0x7f;
        putc(c, out);

        if (count % 6 == 0) {
            // event code: note on, channel 0
            putc(0x90, out);
        }
        else if (count % 6 == 3) {
            // event code: note off, channel 0
            putc(0x80, out);
        }
    }

    // fill in left-over bytes with 0
    for (int i = 0; i < extra; i++) {
        putc(0, out);
    }

    // write all notes off message w/ delta of 7f
    write_int(out, 0x7fb07b00, 4);
    // write end of track message
    write_int(out, 0x00ff2f00, 4);
}

void write_test(FILE *out) {
    uint32_t mtrk = 0x6b72544d;
    fwrite(&mtrk, 1, 4, out);

    uint32_t size = 0x11000000;
    fwrite(&size, 4, 1, out);

    uint32_t on = 0x7f349001;
    uint32_t off = 0x7f34807f;
    // 9c 10 b0 7b 00
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

    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}

#ifndef TRES_H
#define TRES_H

#include <notcurses/notcurses.h>

extern int run;

#define DEBUG_ON    0

#define DEBUG(fmt, ...) \
            do { if (DEBUG_ON) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

typedef enum {
    TCONT,
    TPIC,
    TSCROL,
    TCHOS,
    TTEXT,
    TEND
} tblock_e;

typedef struct tblock {
    struct ncplane* n;
    struct blockdef* bd;
    struct blocksize* bs;
    void* widget;
} tblock;

typedef struct tres {
    struct tblock** tb;
    tblock_e* blocks;
} tres;

#endif

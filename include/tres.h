#ifndef TRES_H
#define TRES_H

#include <notcurses/notcurses.h>

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

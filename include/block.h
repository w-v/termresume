#ifndef BLOCK_H
#define BLOCK_H

#include <notcurses/notcurses.h>

#include "tres.h"

void tblock_destroy(struct tblock* tr);

struct tblock* tblock_init();

typedef struct blockalign {
    struct ncplane* base;
    ncalign_e side;
    ncalign_e bside;
} blockalign;

typedef struct blockdef {
    struct blockalign* align[2];
    float sizerel[2];
    unsigned int sizemin[2];
    unsigned int sizemax[2];
    float padrel[2];
    unsigned int padmin[2];
    unsigned int padmax[2];
    float margrel[2];
    unsigned int margmin[2];
    unsigned int margmax[2];
    char * border;
} blockdef;

typedef struct blocksize {
    unsigned int pos[2];
    unsigned int size[2];
    unsigned int off[2];
    unsigned int marg[2];
} blocksize;

void compute_block(struct ncplane* n, struct blockdef* bd, struct blocksize* bs);

#endif

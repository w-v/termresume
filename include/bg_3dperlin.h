#ifndef BG_3DPERLIN_H
#define BG_3DPERLIN_H

#include "term3d_c/render.h"
#include <notcurses/notcurses.h>

#include "tres.h"

typedef struct bg_3dperlin {
    char** fbuf;
    float* zbuf;
    renderargs* rargs;
    geom* g;
    geom* tg;
    vec3 rot;
    vec3 tr;
    float scale;
    int len;
    int tgeom[2];
} bg_3dperlin;

struct bg_3dperlin* bg_3dperlin_create(struct notcurses* nc);
void* bg_3dperlin_run(struct notcurses* nc, struct tres* tr, struct ncplane* n, struct bg_3dperlin* bg3d);
void bg_3dperlin_destroy(struct bg_3dperlin* bg3d);

#endif


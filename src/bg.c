#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "pic.h"
#include "utils.h"
#include "block.h"
#include "block.h"

struct ncplane* create_bg(struct notcurses* nc, struct tres* tr){

    struct tblock** tb = tr->tb;

    unsigned int tdim[2];

    struct ncplane* nstd = notcurses_stddim_yx(nc, &tdim[0], &tdim[1]);

    /* create block */

    struct ncplane_options nopts = {
        .y = 0,
        .x = 0,
        .rows = tdim[0],
        .cols = tdim[1],
        .userptr = NULL,
        .name = "bg",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(nstd, &nopts);

    ncplane_move_below(n, tb[TCONT]->n);

    ncplane_set_bg_alpha(n, NCALPHA_OPAQUE);
    ncplane_set_base(n," ", 0, 0);

    return n;
}


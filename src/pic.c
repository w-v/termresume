#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "pic.h"
#include "utils.h"
#include "block.h"

struct tblock* create_pic(struct notcurses* nc, struct tres* tr){

    struct tblock** tb = tr->tb;
    struct tblock* tpic = tblock_init();

    /* create block */

    *(tpic->bd) = (struct blockdef) {
        .align = {NULL, NULL},
        .sizerel = {0.3f, 0.3f},
        .sizemin = {10, 20},
        .sizemax = {20, 40},
        .padrel = {0.f, 0.f},
        .padmin = {0, 0},
        .padmax = {0, 0},
        .margrel = {0.f, 0.f},
        .margmin = {0, 0},
        .margmax = {0, 0},
        .border = "┌┐└┘─│",
    };

    compute_block(tb[TCONT]->n, tpic->bd, tpic->bs);

    struct ncplane_options nopts = {
        .y = tpic->bs->pos[0],
        .x = tpic->bs->pos[1],
        .rows = tpic->bs->size[0],
        .cols = tpic->bs->size[1],
        .userptr = NULL,
        .name = "pic",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(tr->tb[TCONT]->n, &nopts);

    /* load picture */

    ncblitter_e blitter = NCBLIT_PIXEL;
    if(notcurses_check_pixel_support(nc) <= 0){
        blitter = NCBLIT_DEFAULT;
    }

    const char *fn = "/home/robin/src/termresume/pic/me.jpg";
    struct ncvisual* ncv = ncvisual_from_file(fn);

    struct ncvisual_options vopts = {
        .n = n,
        .y = 0,
        .x = 0,
        .scaling = NCSCALE_SCALE,
        .blitter = blitter,
        .flags = 0,
    };

    struct ncvgeom ncvg2 = {};
    ncvisual_geom(nc, ncv, &vopts, &ncvg2);

    struct ncplane_options nnopts = {
        .y = 1,
        .x = 1,
        .rows = ncvg2.rcelly,
        .cols = ncvg2.rcellx,
        .userptr = NULL,
        .name = NULL,
        .resizecb = NULL,
        .flags = 0,
    };
    ncplane_resize_simple(n,ncvg2.rcelly+2, ncvg2.rcellx+2);

    ncvisualplane_create(nc, &nnopts, ncv, &vopts);

    struct ncvgeom ncvg1 = {};
    ncvisual_geom(nc, ncv, &vopts, &ncvg1);

    tpic->n = n;

    draw_box(n, NULL, tr);


    return tpic;
}

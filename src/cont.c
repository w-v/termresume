#include <notcurses/notcurses.h>

#include "tres.h"
#include "block.h"
#include "utils.h"

struct tblock* create_cont(struct notcurses* nc, struct tres* tr){

    struct tblock* tcont = tblock_init();
    struct ncplane* nstd = notcurses_stdplane(nc);

    /* create block */

    *(tcont->bd) = (struct blockdef) {
        .align = {NULL, NULL},
        .sizerel = {1.f, 1.f},
        .sizemin = {50, 50},
        .sizemax = {99999, 99999},
        .padrel = {0.f, 0.f},
        .padmin = {0, 0},
        .padmax = {0, 0},
        .margrel = {0.f, 0.f},
        .margmin = {0, 0},
        .margmax = {0, 0},
        .border = "┌┐└┘─│",
    };

    compute_block(nstd, tcont->bd, tcont->bs);

    struct ncplane_options nopts = {
        .y = tcont->bs->pos[0],
        .x = tcont->bs->pos[1],
        .rows = tcont->bs->size[0],
        .cols = tcont->bs->size[1],
        .userptr = NULL,
        .name = "cont",
        .resizecb = NULL,
        .flags = 0,
    };


    struct ncplane* n = ncplane_create(nstd, &nopts);

    /* ncplane_set_bg_alpha(n, NCALPHA_OPAQUE); */
    /* ncplane_set_base(n," ", 0, 0); */
    /* draw_box(n, NULL, tr); */

    tcont->n = n;
    return tcont;
}

void resize_cont(struct notcurses* nc, struct tres* tr){
    struct tblock** tb = tr->tb;
    struct ncplane* n = tb[TCONT]->n;

    int frame [2] = {0,0};
    unsigned int size [2] = {0,0};
    int rframe [2] = {0,0};

    for(tblock_e* b = tr->blocks+1; *b != TEND; b++){

        ncplane_yx(tb[*b]->n, &frame[0], &frame[1]);
        ncplane_dim_yx(tb[*b]->n, &size[0], &size[1]);

        for(int e=0;e<2;e++){
            frame[e] += size[e];
            if(frame[e] > rframe[e]){
                rframe[e] = frame[e];
            }
        }

    }
    // remove old box
    ncplane_erase(n);

    ncplane_resize_simple(n, rframe[0], rframe[1]);

    /* draw_box(n, NULL, tr); */
}

void center_cont(struct notcurses* nc, struct tres* tr){
    struct tblock** tb = tr->tb;

    struct ncplane* nstd = notcurses_stdplane(nc);
    unsigned int 
        size[2] = {0,0},
        mid[2] = {0,0};

    ncplane_dim_yx(nstd, &mid[0], &mid[1]);
    ncplane_dim_yx(tb[TCONT]->n, &size[0], &size[1]);

    for(int e=0;e<2;e++){
        mid[e] = (mid[e] - size[e])/2;
    }
    ncplane_move_yx(tb[TCONT]->n, mid[0], mid[1]);
    
}

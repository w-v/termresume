#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "chos.h"
#include "mselector.h"
#include "utils.h"
#include "block.h"

char* items[] = {
  "About",
  "Work",
  "Education",
  "Skills",
  NULL
};
struct tblock* create_chos(struct notcurses* nc, struct tres* tr){

    struct tblock** tb = tr->tb;
    struct tblock* tchos = tblock_init();

    unsigned int 
        nrows = sizeof(items)/sizeof(char*)-1,
        ncols = maxlen(items)+1;

    /* create block */

    struct blockalign* align = (struct blockalign*) malloc(sizeof(struct blockalign)*2);
    align = (struct blockalign[2]){
            {
                .base = tb[TPIC]->n,
                .side = NCALIGN_TOP,
                .bside = NCALIGN_BOTTOM,
            },
            {
                .base = tb[TPIC]->n,
                .side = NCALIGN_LEFT,
                .bside = NCALIGN_LEFT
            }
        };

    *(tchos->bd) = (struct blockdef) {
        .align = {&align[0], &align[1]},
        .sizerel = {0.f, 0.f},
        .sizemin = {nrows, ncols},
        .sizemax = {nrows, ncols},
        .padrel = {0.f, 0.f},
        .padmin = {1, 2},
        .padmax = {1, 2},
        .margrel = {0.f, 0.f},
        .margmin = {0, 0},
        .margmax = {0, 0},
        .border = "┌┐└┘─│",
    };

    compute_block(tb[TCONT]->n, tchos->bd, tchos->bs);

    struct ncplane_options nopts = {
        .y = tchos->bs->pos[0],
        .x = tchos->bs->pos[1],
        .rows = tchos->bs->size[0],
        .cols = tchos->bs->size[1],
        .userptr = NULL,
        .name = "chos",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(tr->tb[TCONT]->n, &nopts);
    ncplane_set_bg_alpha(n, NCALPHA_OPAQUE);
    ncplane_set_base(n," ", 0, 0);
    tchos->widget = (void*) mselector_create(n, tchos->bs, items);

    tchos->n = n;

    draw_box(n, NULL);
    /* ncplane_move_above(n, tb[TCONT]->n); */
    ncplane_move_top(n);


    return tchos;
}

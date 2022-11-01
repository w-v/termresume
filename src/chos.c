#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "chos.h"
#include "utils.h"
#include "block.h"

static struct ncselector_item items[] = {
#define SITEM(s, l) { s, l, }
  SITEM("About", NULL),
  SITEM("Work", NULL),
  SITEM("Education", NULL),
  SITEM("Skills", NULL),
  SITEM(NULL, NULL),
#undef SITEM
};

struct ncselector* create_selector(struct ncplane* n){
  ncselector_options sopts;
  memset(&sopts, 0, sizeof(sopts));
  sopts.defidx = 0;
  sopts.items = items;
  sopts.opchannels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0);
  return ncselector_create(n, &sopts);
}

struct tblock* create_chos(struct notcurses* nc, struct tres* tr){

    struct tblock** tb = tr->tb;
    struct tblock* tchos = tblock_init();

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
        .sizerel = {0.3f, 0.3f},
        .sizemin = {20, 20},
        .sizemax = {20, 20},
        .padrel = {0.f, 0.f},
        .padmin = {0, 0},
        .padmax = {0, 0},
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
    tchos->widget = (void*) create_selector(n);

    tchos->n = n;

    draw_box(n, NULL, tr);
    /* ncplane_move_above(n, tb[TCONT]->n); */
    ncplane_move_top(n);


    return tchos;
}

void* run_chos(void* args){
    struct chos_args* cargs = (struct chos_args*) args;
    struct notcurses* nc = cargs->nc;
    struct tres* tr = cargs->tr;

    struct tblock** tb = tr->tb;
    struct ncplane* n = tb[TCHOS]->n;
    struct ncselector* ns = (struct ncselector*) tb[TCHOS]->widget;
    
    struct ncinput ni;
    uint32_t keypress;
    while(1){
        keypress = notcurses_get_blocking(nc, &ni);
        /* fprintf(stderr, "keypress %d\n", keypress); */
        ncselector_offer_input(ns, &ni);
        draw_box(n, NULL, tr);
        box_corners(tr);
        /* notcurses_render(nc); */
    }
    ncselector_destroy(ns, NULL);
    return NULL;
}

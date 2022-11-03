#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "chos.h"
#include "utils.h"
#include "block.h"

char* items[] = {
  "About",
  "Work",
  "Education",
  "Skills",
  NULL
};

struct mselector* mselector_create(struct ncplane* n, struct blocksize* bs){
    struct mselector* msel = malloc(sizeof(struct mselector));
    memset(msel, 0, sizeof(*msel));
    msel->sel = 0;
    msel->items = items;
    msel->nitems = sizeof(items)/sizeof(char*)-1;
    
    int y = bs->off[0], x, i = 0;
    char** itm = items;
    while(*itm != NULL){
        if(i == msel->sel){
            ncplane_set_fg_rgb8(n, 0, 0, 0);
            ncplane_set_bg_rgb8(n, 255, 255, 255);
        }
        x = bs->size[1] - strlen(*itm) - bs->off[1];
        ncplane_putstr_yx(n, y++, x, *itm++);
        if(i++ == msel->sel){
            ncplane_set_fg_default(n);
            ncplane_set_bg_default(n);
        }
    }
    return msel;
}

int mselector_select(struct tblock* tb, int nsel){
    struct ncplane* n = tb->n;
    struct mselector* msel = (struct mselector*) tb->widget;
    struct blocksize* bs = tb->bs;

    if(nsel > msel->nitems-1 || nsel < 0){
        return 0;
    }

    int osel = msel->sel;
     
    // remove old highlight
    ncplane_set_fg_default(n);
    ncplane_set_bg_default(n);
    int x = bs->size[1] - strlen(msel->items[osel]) - bs->off[1],
        y = bs->off[0] + osel;
    ncplane_putstr_yx(n, y++, x, msel->items[osel]);

    // add new highlight
    ncplane_set_fg_rgb8(n, 0, 0, 0);
    ncplane_set_bg_rgb8(n, 255, 255, 255);
    x = bs->size[1] - strlen(msel->items[nsel]) - bs->off[1];
    y = bs->off[0] + nsel;
    ncplane_putstr_yx(n, y++, x, msel->items[nsel]);
    msel->sel = nsel;

    return nsel-osel;
}

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
    tchos->widget = (void*) mselector_create(n, tchos->bs);

    tchos->n = n;

    draw_box(n, NULL, tr);
    /* ncplane_move_above(n, tb[TCONT]->n); */
    ncplane_move_top(n);


    return tchos;
}

bool mselector_offer_mice(struct tblock* tb, const ncinput* nc){
    struct ncplane* n = tb->n;
    struct mselector* msel = (struct mselector*) tb->widget;
    struct blocksize* bs = tb->bs;

    if(nc->evtype == NCTYPE_PRESS && (nc->id == NCKEY_BUTTON1 || nc->id == NCKEY_SCROLL_UP || nc->id == NCKEY_SCROLL_DOWN)){
        // mouse event
        int y = nc->y, x = nc->x;
        if(ncplane_translate_abs(n, &y, &x)){
            // inside plane
            if(y >= bs->off[0] && y <= bs->size[0]-bs->off[0]){
                // inside items area
                if(nc->id == NCKEY_BUTTON1){
                    int id = y-bs->off[0];
                    mselector_select(tb, id);
                }
                else if(nc->id == NCKEY_SCROLL_UP){
                    mselector_select(tb, msel->sel-1);
                }
                else if(nc->id == NCKEY_SCROLL_DOWN){
                    mselector_select(tb, msel->sel+1);
                }
                return true;
            }
        }
    }
    return false;

}

bool mselector_offer_kbd(struct tblock* tb, const ncinput* nc){
    struct mselector* msel = (struct mselector*) tb->widget;

    if(nc->evtype != NCTYPE_RELEASE){
        if(nc->id == NCKEY_UP){
            mselector_select(tb, msel->sel-1);
            return true;
        }else if(nc->id == NCKEY_DOWN){
            mselector_select(tb, msel->sel+1);
            return true;
        }else if(nc->id == NCKEY_PGDOWN){
            mselector_select(tb, msel->sel+1);
            return true;
        }else if(nc->id == NCKEY_PGUP){
            mselector_select(tb, msel->sel-1);
            return true;
        }
    }
    return false;
}



void* run_chos(void* args){
    struct chos_args* cargs = (struct chos_args*) args;
    struct notcurses* nc = cargs->nc;
    struct tres* tr = cargs->tr;

    struct tblock** tb = tr->tb;
    struct ncplane* n = tb[TCHOS]->n;
    struct mselector* msel = (struct mselector*) tb[TCHOS]->widget;
    
    struct ncinput ni;
    uint32_t keypress;
    /* while(1){ */
    /*     mselector_select(tb[TCHOS], (msel->sel+1)%msel->nitems); */
    /*     sleep(1); */
    /* } */
    while(1){
        keypress = notcurses_get_blocking(nc, &ni);
        if(!mselector_offer_mice(tb[TCHOS], &ni)){
            mselector_offer_kbd(tb[TCHOS], &ni);
        }
        /* ncselector_offer_input(ns, &ni); */
        /* draw_box(n, NULL, tr); */
        /* box_corners(tr); */
        /* notcurses_render(nc); */
    }
    /* ncselector_destroy(ns, NULL); */
    /* return NULL; */
}

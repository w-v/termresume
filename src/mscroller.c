#include <time.h>
#include <locale.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "mscroller.h"
#include "block.h"
#include "text.h"

void scrollbar_update(struct mscroller* mscr){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    unsigned int dimdum[2];
    ncplane_dim_yx(mscr->ndum, &dimdum[0], &dimdum[1]);

    int bar[2];
    // start
    bar[0] = round((mscr->y/(float)dimdum[0])*dim[0]);
    // size 
    bar[1] = round((dim[0]/(float)dimdum[0])*dim[0]);

    ncplane_erase(mscr->nbar);

    ncplane_cursor_move_yx(mscr->nbar, bar[0], 0);
    unsigned long w;
    while(bar[1]-- > 0){
        ncplane_putegc(mscr->nbar, "█", &w);
        /* ncplane_putchar(mscr->nbar, '0'); */
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
    }

}

void scrollbar_update_half(struct mscroller* mscr){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    unsigned int dimdum[2];
    ncplane_dim_yx(mscr->ndum, &dimdum[0], &dimdum[1]);

    int bar[2];
    // start
    bar[0] = round((mscr->y/(float)dimdum[0])*dim[0]*2);
    // size 
    bar[1] = round((dim[0]/(float)dimdum[0])*dim[0]);

    ncplane_erase(mscr->nbar);

    ncplane_cursor_move_yx(mscr->nbar, bar[0]/2, 0);
    unsigned long w;
    if(bar[0]%2 == 1){
        ncplane_putegc(mscr->nbar, "▄", &w);
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
        bar[1]--;
    }
    while(bar[1]-- > 0){
        ncplane_putegc(mscr->nbar, "█", &w);
        /* ncplane_putchar(mscr->nbar, '0'); */
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
    }
    if(bar[0]%2 == 1){
        ncplane_putegc(mscr->nbar, "▀", &w);
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
    }

    /* int a = 0; */
    /* ncplane_cursor_move_yx(mscr->nbar, 0, 1); */
    /* while(a++ < dim[1]){ */
    /*     ncplane_putegc(mscr->nbar, "▀", &w); */
    /*     ncplane_putegc(mscr->nbar, "▄", &w); */
    /*     ncplane_cursor_move_rel(mscr->nbar, 1, -2); */
    /* } */

}

void scrollbar_show(struct mscroller* mscr, bool show){
    if(mscr->sbar_show != show){
        mscr->sbar_show = show;
        if(show){
            scrollbar_update_half(mscr);
        }
        else{
            ncplane_erase(mscr->nbar);
        }
    }
}

void mscroller_scrollto(struct mscroller* mscr, int y){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    if(y >= 0 && y <= mscr->maxscroll-dim[0]){
        mscr->y = y;
        if(mscr->nbar != NULL && mscr->sbar_show){
            scrollbar_update_half(mscr);
        }
        char* subtxt = ncplane_contents(mscr->ndum, y, 0, dim[0], dim[1]);
        ncplane_erase(mscr->n);
        ncplane_puttext(mscr->n, 0, NCALIGN_LEFT,subtxt, NULL);
    }
}

bool mscroller_offer_mice(struct mscroller* mscr, const ncinput* nc){

    if(nc->evtype == NCTYPE_PRESS && (nc->id == NCKEY_BUTTON1 || nc->id == NCKEY_SCROLL_UP || nc->id == NCKEY_SCROLL_DOWN)){
        // mouse event
        int y = nc->y, x = nc->x;
        if(ncplane_translate_abs(mscr->n, &y, &x)){
            // inside plane
            if(nc->id == NCKEY_SCROLL_UP){
                mscroller_scrollto(mscr, mscr->y-1);
            }
            else if(nc->id == NCKEY_SCROLL_DOWN){
                mscroller_scrollto(mscr, mscr->y+1);
            }
            return true;
        }
    }
    return false;


}

bool mscroller_offer_kbd(struct mscroller* mscr, const ncinput* nc){

    if(nc->evtype != NCTYPE_RELEASE){
        if(nc->id == NCKEY_UP){
            mscroller_scrollto(mscr, mscr->y-1);
            return true;
        }else if(nc->id == NCKEY_DOWN){
            mscroller_scrollto(mscr, mscr->y+1);
            return true;
        }
    }
    return false;
}

struct ncplane* dummyplane_create(struct ncplane* n, char* text){
    struct ncplane* ndum = ncplane_dup(n, NULL);
    ncplane_set_autogrow(ndum, 1);
    ncplane_set_scrolling(ndum, 0);
    ncplane_move_yx(ndum, 0, 0);
    ncplane_puttext(ndum, 0, NCALIGN_LEFT,text, NULL);
    ncplane_move_bottom(ndum);
    return ndum;
}


struct ncplane* scrollbar_create(struct ncplane* parent, struct ncplane* n){
    int pos[2];
    unsigned int dim[2];
    ncplane_dim_yx(n, &dim[0], &dim[1]);
    ncplane_yx(n, &pos[0], &pos[1]);
    struct ncplane_options nopts = {
            .y = pos[0],
            .x = pos[1]+dim[1]+1,
            .rows = dim[0],
            .cols = 10,
            .userptr = NULL,
            .name = NULL,
            .resizecb = NULL,
            .flags = 0,
        };
    struct ncplane* nbar = ncplane_create(parent, &nopts);
    return nbar;
}


/* int main(void){ */

/*     struct notcurses_options opts = { */
/*         .flags = 0 */
/*     }; */
/*     struct notcurses* nc = notcurses_init(&opts, NULL); */
/*     if(nc == NULL){ */
/*         return EXIT_FAILURE; */
/*     } */
/*     unsigned dimy, dimx; */
/*     struct ncplane* nstd = notcurses_stddim_yx(nc, &dimy, &dimx); */
/*     struct ncplane_options nopts = { */
/*         .y = NCALIGN_CENTER, */
/*         .x = NCALIGN_CENTER, */
/*         .rows = 20, */
/*         .cols = 40, */
/*         .userptr = NULL, */
/*         .name = NULL, */
/*         .resizecb = NULL, */
/*         .flags = NCPLANE_OPTION_HORALIGNED | NCPLANE_OPTION_VERALIGNED, */
/*     }; */
/*     struct ncplane* n = ncplane_create(nstd, &nopts); */

/*     struct ncplane* ndum = dummyplane_create(n, entries[0]); */

/*     struct mscroller mscr = { */
/*         .y = 0, */
/*         .n = n, */
/*         .ndum = ndum, */
/*         .nbar = scrollbar_create(nstd, n), */
/*         .maxscroll = ncplane_dim_y(ndum) */
/*     }; */
/*     mscroller_scrollto(&mscr, mscr.y); */
    
/*     notcurses_render(nc); */


/*     struct ncinput ni; */
/*     uint32_t keypress; */
/*     while(1){ */
/*         keypress = notcurses_get_blocking(nc, &ni); */
/*         mscroller_offer_kbd(&mscr, &ni); */
/*         notcurses_render(nc); */
/*     } */

/*     sleep(1000); */

/*     notcurses_stop(nc); */
/*     return EXIT_SUCCESS; */
/* } */




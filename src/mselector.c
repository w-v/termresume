#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "utils.h"
#include "block.h"
#include "mselector.h"

struct mselector* mselector_create(struct ncplane* n, struct blocksize* bs, char* items[]){
    struct mselector* msel = malloc(sizeof(struct mselector));
    memset(msel, 0, sizeof(*msel));
    msel->sel = 0;
    msel->items = items;
    msel->nitems = 0;
    for(char** i = items; *i != NULL;i++)  msel->nitems++;
    
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

void mselector_unselect(struct tblock* tb){
    struct ncplane* n = tb->n;
    struct mselector* msel = (struct mselector*) tb->widget;
    struct blocksize* bs = tb->bs;

    int osel = msel->sel;
     
    // remove old highlight
    ncplane_set_fg_default(n);
    ncplane_set_bg_default(n);
    int x = bs->size[1] - strlen(msel->items[osel]) - bs->off[1],
        y = bs->off[0] + osel;
    ncplane_putstr_yx(n, y++, x, msel->items[osel]);
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

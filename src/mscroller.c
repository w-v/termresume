#include <time.h>
#include <locale.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "mscroller.h"
#include "block.h"
#include "text.h"

char* barchars_default[] = {"▄",  "█", "▀"};
char* barchars_shade[] =  {"░", "░", "░"};

char* arrowschars_default[] = {"▲", "▼"};

void line_break(char* t, int len){
    int x = 0, lb;
    for(int i = 0;t[i] != '\0';i++, x++){
        if(t[i] == ' '){
            lb = i;
        }
        else if(t[i] == '\n'){
            x = -1; // 0 at next iter
        }
        if(x == len-1){
            t[lb] = '\n';
            x = i-lb;
        }
    }
}

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

void scrollbar_arrows(struct mscroller* mscr, char** arrowschars_, bool up, bool down){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->nbar, &dim[0], &dim[1]);

    char** arrowschars;

    if(arrowschars_ == NULL){
        arrowschars = arrowschars_default;
    }
    else {
        arrowschars = arrowschars_;
    }

    if(up){
        ncplane_cursor_move_yx(mscr->nbar, 0, 0);
        ncplane_putegc(mscr->nbar, arrowschars[0], NULL);
    }

    if(down){
        ncplane_cursor_move_yx(mscr->nbar, ncplane_dim_y(mscr->nbar)-1, 0);
        ncplane_putegc(mscr->nbar, arrowschars[1], NULL);
    }

}

void scrollbar_update_half(struct mscroller* mscr, char** barchars_){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    unsigned int dimdum[2];
    ncplane_dim_yx(mscr->ndum, &dimdum[0], &dimdum[1]);
    char** barchars;

    if(barchars_ == NULL){
        barchars = barchars_default;
    }
    else {
        barchars = barchars_;
    }

    int bar[2];
    // start
    bar[0] = round((mscr->y/(float)dimdum[0])*(dim[0]-2)*2);
    // size 
    bar[1] = round((dim[0]/(float)dimdum[0])*(dim[0]-2));

    ncplane_erase(mscr->nbar);

    ncplane_cursor_move_yx(mscr->nbar, bar[0]/2+1, 0);
    unsigned long w;
    if(bar[0]%2 == 1){
        ncplane_putegc(mscr->nbar, barchars[0], &w);
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
        bar[1]--;
    }
    while(bar[1]-- > 0){
        ncplane_putegc(mscr->nbar, barchars[1], &w);
        /* ncplane_putchar(mscr->nbar, '0'); */
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
    }
    if(bar[0]%2 == 1){
        ncplane_putegc(mscr->nbar, barchars[2], &w);
        ncplane_cursor_move_yx(mscr->nbar, -1, 0);
        ncplane_cursor_move_rel(mscr->nbar, 1, 0);
    }

    scrollbar_arrows(
            mscr, 
            NULL, 
            mscr->y > 0,
            mscr->y < mscr->maxscroll-dim[0]
        );

}

void scrollbar_show(struct mscroller* mscr, bool show){
    if(mscr->nbar != NULL){
        if(mscr->sbar_show != show){
            mscr->sbar_show = show;
            if(show){
                scrollbar_update_half(mscr, NULL);
            }
            else{
                scrollbar_update_half(mscr, barchars_shade);
                /* ncplane_erase(mscr->nbar); */
            }
        }
    }
}

struct ncplane* scrollbar_create(struct ncplane* parent, struct ncplane* n){
    int pos[2];
    unsigned int dim[2];
    ncplane_dim_yx(n, &dim[0], &dim[1]);
    ncplane_yx(n, &pos[0], &pos[1]);
    struct ncplane_options nopts = {
            .y = pos[0],
            .x = pos[1]+dim[1],
            .rows = dim[0],
            .cols = 10,
            .userptr = NULL,
            .name = NULL,
            .resizecb = NULL,
            .flags = 0,
        };
    struct ncplane* nbar = ncplane_create(parent, &nopts);
    return nbar;
    ncplane_set_bg_alpha(n, NCALPHA_BLEND);
}


void mscroller_scrollto(struct mscroller* mscr, int y){
    unsigned int dim[2];
    char* subtxt;
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    if(y >= 0 && y <= mscr->maxscroll-dim[0]){
        if(y > 0 && !mscr->scrolling){
            // don't scroll
            // (allow y=0 to display first lines)
            return;
        }
        mscr->y = y;
        if(mscr->nbar != NULL){
            if(mscr->sbar_show){
                scrollbar_update_half(mscr, NULL);
            }
            else {
                scrollbar_update_half(mscr, barchars_shade);
            }
        }
        ncplane_erase(mscr->n);
        for(int i = 0; i< dim[0]; i++){
            subtxt = ncplane_contents(mscr->ndum, i+y, 0, 1, dim[1]);
            ncplane_cursor_move_yx(mscr->n, i, 0);
            ncplane_printf_aligned(mscr->n, i, NCALIGN_LEFT,subtxt);
        }
    }
}

void mscroller_scrollto_puttext(struct mscroller* mscr, int y){
    unsigned int dim[2];
    ncplane_dim_yx(mscr->n, &dim[0], &dim[1]);
    if(y >= 0 && y <= mscr->maxscroll-dim[0]){
        mscr->y = y;
        if(mscr->nbar != NULL){
            if(mscr->sbar_show){
                scrollbar_update_half(mscr, NULL);
            }
            else {
                scrollbar_update_half(mscr, NULL);
            }
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
        if(nc->id == NCKEY_UP || nc->id == 'k'){
            mscroller_scrollto(mscr, mscr->y-1);
            return true;
        }else if(nc->id == NCKEY_DOWN || nc->id == 'j'){
            mscroller_scrollto(mscr, mscr->y+1);
            return true;
        }
    }
    return false;
}

struct ncplane* dummyplane_create(struct ncplane* n, char* text){
    struct ncplane* ndum = ncplane_dup(n, NULL);
    ncplane_set_autogrow(ndum, 1);
    ncplane_set_scrolling(ndum, 1);
    ncplane_reparent(ndum, ncplane_parent(ncplane_parent(ndum)));
    ncplane_move_yx(ndum, 0, 0);
    ncplane_resize_simple(ndum, 1, ncplane_dim_x(ndum));
    ncplane_printf_aligned(ndum, 0, NCALIGN_LEFT, text);
    ncplane_move_bottom(ndum);
    return ndum;
}

struct ncplane* dummyplane_create_puttext(struct ncplane* n, char* text){
    struct ncplane* ndum = ncplane_dup(n, NULL);
    ncplane_set_autogrow(ndum, 1);
    ncplane_set_scrolling(ndum, 0);
    ncplane_move_yx(ndum, 0, 0);
    ncplane_puttext(ndum, 0, NCALIGN_LEFT,text, NULL);
    ncplane_move_bottom(ndum);
    return ndum;
}



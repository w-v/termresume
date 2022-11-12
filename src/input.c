#include "tres.h"
#include "text.h"
#include "chos.h"
#include "mscroller.h"
#include "mselector.h"
#include "input.h"

void tblock_select(struct tblock** tb, struct tblock* st){
    struct mselector* msel = (struct mselector*) tb[TCHOS]->widget;
    struct textitem** titems = (struct textitem**) tb[TTEXT]->widget;
    if(st == tb[TCHOS]){
        // unselect text
        scrollbar_show(titems[msel->sel]->mscr, false);
        // select chos
        mselector_select(tb[TCHOS], msel->sel);
    }
    else if(st == tb[TTEXT]){
        // select text
        scrollbar_show(titems[msel->sel]->mscr, true);
        // unselect chos
        mselector_unselect(tb[TCHOS]);
    }
}

bool input_offer_kbd(struct tblock** tb, struct tblock* selected, const ncinput* nc){
    if(nc->evtype != NCTYPE_RELEASE){
        if(nc->id == NCKEY_LEFT || nc->id == NCKEY_RIGHT){

        }
    }
    return false;

}

void* input_run(void* args){
    struct input_args* iargs = (struct input_args*) args;
    struct notcurses* nc = iargs->nc;
    struct tres* tr = iargs->tr;

    struct tblock** tb = tr->tb;
    /* struct ncplane* n = tb[TCHOS]->n; */
    struct mselector* msel = (struct mselector*) tb[TCHOS]->widget;
    struct textitem** titems = (struct textitem**) tb[TTEXT]->widget;
    
    struct ncinput ni;
    uint32_t keypress;
    struct tblock* selected = tb[TCHOS];
    tblock_select(tb, selected);
    while(1){
        keypress = notcurses_get_blocking(nc, &ni);

        if(titems[msel->sel]->mscr->scrolling && mscroller_offer_mice(titems[msel->sel]->mscr, &ni)){
            selected = tb[TTEXT];
            tblock_select(tb, selected);
        }
        else if(mselector_offer_mice(tb[TCHOS], &ni)){
            selected = tb[TCHOS];
            tblock_select(tb, selected);
        }
        else if(titems[msel->sel]->mscr->scrolling && ni.evtype != NCTYPE_RELEASE){
            if((ni.id == NCKEY_RIGHT || ni.id == NCKEY_ENTER) && selected == tb[TCHOS]){
                selected = tb[TTEXT];
                tblock_select(tb, selected);
            }
            else if((ni.id == NCKEY_LEFT || ni.id == NCKEY_ESC) && selected == tb[TTEXT]){
                selected = tb[TCHOS];
                tblock_select(tb, selected);
            }
        }
        if(selected == tb[TCHOS]){
            mselector_offer_kbd(tb[TCHOS], &ni);
        }
        else if(selected == tb[TTEXT]){
            mscroller_offer_kbd(titems[msel->sel]->mscr, &ni);
        }
    }
}



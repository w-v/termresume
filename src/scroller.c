#include <time.h>
#include <scroller.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "utils.h"
#include "block.h"

int nanosleep(const struct timespec *req, struct timespec *rem);

char* lines[] = { 
    "Hey, I'm Robin 👋",
    "I'm a software engineer",
    /* "I'm a research engineer",       // get research blinking replacing by software */
    "I like Computer Graphics,",
    "Python, and terminals",
    "This is my homepage 🏡",
    "",
    "Browse it from your terminal:",
    "",
    "ssh -p 4222 fr@robinadi.li",
    NULL
};


struct tblock* create_scroller(struct tres* tr){
    struct tblock** tb = tr->tb;
    struct tblock* tscrol = tblock_init();

    unsigned int 
        nrows = sizeof(lines)/sizeof(char*)-1,
        ncols = maxlen(lines)+1;


    struct blockalign* align = (struct blockalign*) malloc(sizeof(struct blockalign)*2);
    align = (struct blockalign[2]){
            {
                .base = tb[TPIC]->n,
                .side = NCALIGN_BOTTOM,
                .bside = NCALIGN_BOTTOM
            },
            {
                .base = tb[TPIC]->n,
                .side = NCALIGN_LEFT,
                .bside = NCALIGN_RIGHT
            }
        };

    *(tscrol->bd) = (struct blockdef) {
        .align = {&align[0], &align[1]},
        .sizerel = {0.1f, 0.3f},
        .sizemin = {nrows, ncols},
        .sizemax = {nrows, ncols},
        .padrel = {0.05f, 0.05f},
        .padmin = {1, 2},
        .padmax = {1, 2},
        .margrel = {0.f, 0.f},
        .margmin = {0, 0},
        .margmax = {0, 0},
        .border = "┌┐└┘─│",
    };

    compute_block(tb[TCONT]->n, tscrol->bd, tscrol->bs);

    struct ncplane_options nopts = {
        .y = tscrol->bs->pos[0],
        .x = tscrol->bs->pos[1],
        .rows = tscrol->bs->size[0],
        .cols = tscrol->bs->size[1],
        .userptr = NULL,
        .name = "scroller",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(tb[TCONT]->n, &nopts);

    ncplane_set_bg_alpha(n, NCALPHA_OPAQUE);
    ncplane_set_base(n," ", 0, 0);
    draw_box(n, NULL, tr);

    ncplane_cursor_move_yx(n, tscrol->bs->off[0], tscrol->bs->off[1]);

    tscrol->n = n;

    return tscrol; 
}

void animate(struct notcurses* nc, struct tres* tr){
    struct tblock** tb = tr->tb;
    struct ncplane* n = tb[TSCROL]->n;
    unsigned int size[2];
    ncplane_dim_yx(n, &size[0], &size[1]);
    int nsec = 8000000;
    struct timespec pause = { .tv_sec = 0, .tv_nsec = nsec, };

    for(int i=2; i<size[1]+1; i++){
        ncplane_erase(n);
        
        //for(int i=2; i<size[1]+1; i++){
        // right
        /* ncplane_resize( */
        /*     n, */
        /*     0,0, */        
        /*     size[0],1, */
        /*     0,0, */
        /*     size[0],i */
        /* ); */
        ncplane_resize_simple(n, size[0], i);
        
        //left
        //for(int i=2; i<size[1]+1; i++){
        /* ncplane_resize_simple(n, size[0], i); */
        /* ncplane_move_rel(n, 0, -1); */

        //top
        //for(int i=2; i<size[0]+1; i++){
        /* ncplane_resize_simple(n, i, size[1]); */
        /* ncplane_move_rel(n, -1, 0); */
        
        // bottom
        //for(int i=2; i<size[0]+1; i++){
        /* ncplane_resize_simple(n, i, size[1]); */

        draw_box(n,NULL, tr);
        box_corners(tr);
        /* notcurses_render(nc); */
        nanosleep(&pause, NULL);
        /* sleep(1); */
    }
    pause.tv_nsec *= 10;
    nanosleep(&pause, NULL);
}

void* scroller(void* args){

    struct scroller_args* sargs = (struct scroller_args*) args;
    struct notcurses* nc = sargs->nc;
    struct tres* tr = sargs->tr;

    struct tblock** tb = tr->tb;
    struct ncplane* n = tb[TSCROL]->n;

    unsigned int* off = tb[TSCROL]->bs->off;
    int l = 0;
    char* c = lines[l];
    int nsec = 40000000;
    struct timespec pauses[] = {
        { .tv_sec = 0, .tv_nsec = nsec, },
        { .tv_sec = 0, .tv_nsec = nsec*2, },
        { .tv_sec = 0, .tv_nsec = nsec*4, }
    };
    size_t w = 0;

    animate(nc,tr);

    ncplane_cursor_move_yx(n, off[0], off[1]);

    while(true){
        if(*c == ','){
            nanosleep(&pauses[1], NULL);
        }
        else {
            nanosleep(&pauses[0], NULL);
        }
        if(*c != '\0'){
            if(ncplane_putegc(n, c, &w) == -1){
                break;
            }
            c += w;
        }
        else {
            if(lines[++l] == NULL){
                break;
            }
            ncplane_putchar(n, ' ');
            ncplane_cursor_move_yx(n, -1, off[1]);
            ncplane_cursor_move_rel(n, 1, 0);
            c = lines[l];
            nanosleep(&pauses[2], NULL);
        }
        // don't render cursor after emojis
        // cursor stuck in half emoji in xtermjs otherwise
        if(w == 1){
            ncplane_set_bg_rgb8(n, 255, 255, 255);
            ncplane_putchar(n, ' ');
            ncplane_set_bg_default(n);
            ncplane_cursor_move_rel(n, 0, -1);
        }

        /* notcurses_render(nc); */
    }
    ncplane_putchar(n, ' ');
    /* notcurses_render(nc); */
    return NULL;
}


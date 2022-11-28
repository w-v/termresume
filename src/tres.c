#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <pthread.h>
#include <signal.h>
#include <notcurses/notcurses.h>

#include "pic.h"
#include "chos.h"
#include "scroller.h"
#include "utils.h"
#include "block.h"
#include "cont.h"
#include "bg.h"
#include "bg_3dperlin.h"
#include "text.h"
#include "input.h"
#include "tres.h"

int run = 1;

void order_z(struct notcurses* nc, struct tres* tr){
    struct tblock** tb = tr->tb;
    ncplane_move_above(tb[TPIC]->n, tb[TCONT]->n);
    ncplane_move_above(tb[TCHOS]->n, tb[TPIC]->n);
    ncplane_move_above(tb[TTEXT]->n, tb[TCHOS]->n);
    ncplane_move_above(tb[TSCROL]->n, tb[TTEXT]->n);
}

void check_mingeom(struct notcurses* nc){
    unsigned int tgeom[2], cgeom[2];
    struct ncplane* nstd = notcurses_stddim_yx(nc, &tgeom[0], &tgeom[1]);

    /* ncplane_dim_yx(tr->tb[TCONT]->n, &cgeom[0], &cgeom[1]); */
    cgeom[0] = 37;
    cgeom[1] = 77;

    if(tgeom[0] < cgeom[0] || tgeom[1] < cgeom[1]){
        struct ncplane_options nopts = {
            .y = NCALIGN_CENTER,
            .x = NCALIGN_CENTER,
            .rows = tgeom[0],
            .cols = tgeom[1],
            .userptr = NULL,
            .name = "mingeom_warn",
            .resizecb = NULL,
            .flags = NCPLANE_OPTION_AUTOGROW | NCPLANE_OPTION_HORALIGNED | NCPLANE_OPTION_VERALIGNED
        };

        struct ncplane* n = ncplane_create(nstd, &nopts);
        struct timespec reloadt = { .tv_sec = 0, .tv_nsec = 100 * 1000000, };

        while((tgeom[0] < cgeom[0] || tgeom[1] < cgeom[1]) && run){
            ncplane_dim_yx(nstd, &tgeom[0], &tgeom[1]);
            ncplane_erase(n);
            ncplane_printf_aligned(n, (int)(tgeom[0]/2), NCALIGN_CENTER, "Terminal must be at least %dx%d, currently %dx%d, please resize\n", cgeom[0], cgeom[1], tgeom[0], tgeom[1]);
            notcurses_render(nc);
            /* sleep(1); */
            nanosleep(&reloadt, NULL);
        }
        ncplane_destroy(n);
    }
}

void run_handler(int _){
    DEBUG("%s\n", "run_handler");
    run = 0;
}

int main(void){

    // kitty ugly fix
    char* term = getenv("TERM");
    if(term != NULL && strcmp(term, "xterm-kitty") == 0){
        setenv("TERM", "kitty", 1);
        DEBUG("%s\n%s\n", "TERM is set to xterm-kitty, which is not currently available in ncurses-terminfo-base, changing it to kitty", "see https://github.com/kovidgoyal/kitty/issues/879");
    }

    signal(SIGABRT, run_handler);
    signal(SIGBUS, run_handler);
    signal(SIGFPE, run_handler);
    signal(SIGILL, run_handler);
    signal(SIGINT, run_handler);
    signal(SIGQUIT, run_handler);
    signal(SIGSEGV, run_handler);
    signal(SIGTERM , run_handler);

   struct notcurses_options opts = {
        .flags = NCOPTION_NO_QUIT_SIGHANDLERS,
        /* .loglevel = NCLOGLEVEL_DEBUG */
    };

    struct notcurses* nc = notcurses_init(&opts, NULL);
    if(nc == NULL){
        return EXIT_FAILURE;
    }
    notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT);

    check_mingeom(nc);

    int nblocks = 6;

    struct tblock** tb = malloc(nblocks * sizeof(tblock*));
    memset(tb, 0, nblocks * sizeof(tblock*));

    tblock_e* blocks = malloc(nblocks * sizeof(tblock_e));
    memcpy(blocks, (tblock_e []){TCONT, TPIC, TSCROL, TCHOS, TTEXT, TEND}, nblocks*sizeof(tblock_e));

    struct tres* tr = &(struct tres){
        .tb = tb,
        .blocks = blocks,
    };

    tb[TCONT] = create_cont(nc, tr);

    tb[TPIC] = create_pic(nc, tr);

    tb[TSCROL] = create_scroller(tr);

    tb[TCHOS] = create_chos(nc, tr);

    tb[TTEXT] = create_text(nc, tr);

    order_z(nc, tr);

    box_corners(tr);

    resize_cont(nc, tr);
    center_cont(nc, tr);


    struct ncplane* nbg = create_bg(nc, tr);
    struct bg_3dperlin* bg3d = bg_3dperlin_create(nc);
    

    pthread_t scroller_thread;
    struct scroller_args sargs = {
        .nc = nc,
        .tr = tr
    };

    pthread_create(&scroller_thread, NULL, scroller, &sargs);

    pthread_t input_thread;
    struct input_args iargs = {
        .nc = nc,
        .tr = tr
    };

    pthread_create(&input_thread, NULL, input_run, &iargs);

    DEBUG("%s\n", "bg_3dperlin_run");
    bg_3dperlin_run(nc, tr, nbg, bg3d);

    DEBUG("%s\n", "join scroller");
    pthread_join(scroller_thread, NULL);

    DEBUG("%s\n", "join input");
    pthread_cancel(input_thread);
    /* pthread_join(input_thread, NULL); */

    DEBUG("%s\n", "ending");

    ncplane_destroy(nbg);
    bg_3dperlin_destroy(bg3d);
    DEBUG("%s\n", "destroy");
    destroy_cont(tb[TCONT]);
    destroy_pic(tb[TPIC]);
    destroy_scroller(tb[TSCROL]);
    destroy_chos(tb[TCHOS]);
    destroy_text(tb[TTEXT]);
    DEBUG("%s\n", "free");
    free(tb);
    free(blocks);
    notcurses_stop(nc);
    return EXIT_SUCCESS;
}

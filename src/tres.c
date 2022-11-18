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

void check_mingeom(struct notcurses* nc, struct tres* tr){
    unsigned int tgeom[2], cgeom[2];
    struct ncplane* nstd = notcurses_stddim_yx(nc, &tgeom[0], &tgeom[1]);

    ncplane_dim_yx(tr->tb[TCONT]->n, &cgeom[0], &cgeom[1]);

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

        ncplane_printf_aligned(n, tgeom[0]/2, NCALIGN_CENTER, "Terminal must be at least %dx%d, please resize and reload", cgeom[0], cgeom[1]);
        notcurses_render(nc);
        while(1) sleep(1);
        
    }
}

void run_handler(int _){
    fprintf(stderr, "run_handler\n");
    run = 0;
}

int main(void){

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

    check_mingeom(nc,tr);



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

    fprintf(stderr, "bg_3dperlin_run\n");
    bg_3dperlin_run(nc, tr, nbg, bg3d);

    fprintf(stderr, "join scroller\n");
    pthread_join(scroller_thread, NULL);

    fprintf(stderr, "join input\n");
    pthread_cancel(input_thread);
    /* pthread_join(input_thread, NULL); */

    fprintf(stderr, "ending\n");

    ncplane_destroy(nbg);
    bg_3dperlin_destroy(bg3d);
    fprintf(stderr, "destroy\n");
    destroy_cont(tb[TCONT]);
    destroy_pic(tb[TPIC]);
    destroy_scroller(tb[TSCROL]);
    destroy_chos(tb[TCHOS]);
    destroy_text(tb[TTEXT]);
    fprintf(stderr, "free\n");
    free(tb);
    free(blocks);
    notcurses_stop(nc);
    return EXIT_SUCCESS;
}

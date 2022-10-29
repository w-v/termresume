#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <pthread.h>
#include <notcurses/notcurses.h>

#include "pic.h"
#include "chos.h"
#include "scroller.h"
#include "utils.h"
#include "block.h"
#include "cont.h"
#include "bg.h"
#include "bg_3dperlin.h"

int main(void){
    struct notcurses_options opts = {
        .flags = 0,
        .loglevel = NCLOGLEVEL_DEBUG
    };

    struct notcurses* nc = notcurses_init(&opts, NULL);
    if(nc == NULL){
        return EXIT_FAILURE;
    }
    /* notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT); */

    int nblocks = 5;

    struct tblock** tb = malloc(nblocks * sizeof(tblock*));
    memset(tb, 0, nblocks * sizeof(tblock*));

    tblock_e* blocks = malloc(nblocks * sizeof(tblock_e));
    memcpy(blocks, (tblock_e []){TCONT, TPIC, TSCROL, TCHOS, TEND}, nblocks*sizeof(tblock_e));

    struct tres* tr = &(struct tres){
        .tb = tb,
        .blocks = blocks,
    };

    tb[TCONT] = create_cont(nc, tr);

    tb[TPIC] = create_pic(nc, tr);

    tb[TSCROL] = create_scroller(tr);

    tb[TCHOS] = create_chos(nc, tr);

    resize_cont(nc, tr);
    center_cont(nc, tr);

    box_corners(tr);


    struct ncplane* nbg = create_bg(nc, tr);
    struct bg_3dperlin* bg3d = bg_3dperlin_create(nc);
    bg_3dperlin_run(nc, tr, nbg, bg3d);

/*     pthread_t scroller_thread; */
/*     struct scroller_args sargs = { */
/*         .nc = nc, */
/*         .tr = tr */
/*     }; */

/*     pthread_create(&scroller_thread, NULL, scroller, &sargs); */
/*     pthread_join(scroller_thread, NULL); */

/*     pthread_t chos_thread; */
/*     struct chos_args cargs = { */
/*         .nc = nc, */
/*         .tr = tr */
/*     }; */

    /* pthread_create(&chos_thread, NULL, run_chos, &cargs); */
    /* run_chos((void*) &cargs); */

    /* pthread_join(chos_thread, NULL); */

    sleep(100);
    bg_3dperlin_destroy(bg3d);
    tblock_destroy(tb[TCONT]);
    tblock_destroy(tb[TPIC]);
    tblock_destroy(tb[TSCROL]);
    tblock_destroy(tb[TCHOS]);
    notcurses_stop(nc);
    return EXIT_SUCCESS;
}

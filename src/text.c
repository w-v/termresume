
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "text.h"
#include "utils.h"
#include "block.h"
#include "mscroller.h"

char* entries[] = {
    "I have a passion for computers science and computer graphics, I studied software engineering before shifting to research. Today I'm looking to learn and grow my skills in the industry",
    "Research Engineer      2020-22\r\
Research centered around 3D character animation and virtual crowds for video game and movie applications\n\
Collaboration and technical assistance of PhDs and researchers\n\
Lead developer of an open source 3D character animation research framework in Python, OpenGL and Qt (PyNimation, link below)\n\
Usage and promotion of software engineering tools and methods: versioning, tests, documentation, code quality, CI/CD, dependency management\n\
Full-time management of a software development intern working on the PyNimation project",
    "Virtual human crowds\n are regularly featured in movies and video games. With a large number of virtual characters each behaving in their own way, spectacular scenes can be produced. The more diverse the characters and their behaviors are, the more realistic the virtual crowd is expected to be perceived. Hence, creating virtual crowds is a trade-off between the cost associated with acquiring more diverse assets, namely more virtual characters with their animations, and achieving better realism. In this paper, our focus is on the perceived variety in virtual crowd character motions. We present an experiment exploring whether observers are able to identify virtual crowds including motion clones in the case of large-scale crowds (from 250 to 1000 characters). As it is not possible to acquire individual motions for such numbers of characters, we rely on a state-of-the-art motion variation approach to synthesize unique variations of existing examples for each character in the crowd. Participants then compared pairs of videos, where each character was animated either with a unique motion or using a subset of these motions. Our results show that virtual crowds with more than two motions (one per gender) were perceptually equivalent, regardless of their size. We believe these findings can help create efficient crowd applications, and are an additional step into a broader understanding of the perception of motion variety.",
};

struct tblock* create_text(struct notcurses* nc, struct tres* tr){

    struct tblock** tb = tr->tb;
    struct tblock* ttext = tblock_init();

    /* create block */
    struct blockalign* align = (struct blockalign*) malloc(sizeof(struct blockalign)*2);
    align = (struct blockalign[2]){
            {
                .base = tb[TPIC]->n,
                .side = NCALIGN_TOP,
                .bside = NCALIGN_BOTTOM,
            },
            {
                .base = tb[TCHOS]->n,
                .side = NCALIGN_LEFT,
                .bside = NCALIGN_RIGHT
            }
        };

    *(ttext->bd) = (struct blockdef) {
        .align = {&align[0], &align[1]},
        .sizerel = {0.3f, 0.3f},
        .sizemin = {10, 56},
        .sizemax = {20, 56},
        .padrel = {0.f, 0.f},
        .padmin = {1, 2},
        .padmax = {1, 2},
        .margrel = {0.f, 0.f},
        .margmin = {0, 0},
        .margmax = {0, 0},
        .border = "┌┐└┘─│",
    };


    compute_block(tb[TCONT]->n, ttext->bd, ttext->bs);

    struct ncplane_options nopts = {
        .y = ttext->bs->pos[0],
        .x = ttext->bs->pos[1],
        .rows = ttext->bs->size[0],
        .cols = ttext->bs->size[1],
        .userptr = NULL,
        .name = "text",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(tr->tb[TCONT]->n, &nopts);
    ncplane_move_above(n, tb[TCONT]->n);

    /* load textture */

    ttext->n = n;

    ncplane_set_bg_alpha(n, NCALPHA_OPAQUE);
    ncplane_set_base(n," ", 0, 0);

    draw_box(n, NULL, tr);

    struct textitem* titem = create_textitem(ttext, entries[2]);
    ttext->widget = titem;

    return ttext;
}

struct textitem* create_textitem(struct tblock* ttext, char* text){
    
    struct textitem* titem = malloc(sizeof(textitem));
    memset(titem, 0, sizeof(textitem));

    titem->text = text;

    unsigned int size[2] = {ttext->bs->size[0]-ttext->bs->off[0]-1, ttext->bs->size[1]-ttext->bs->off[1]*2};

    struct ncplane_options nopts = {
        .y = ttext->bs->off[0],
        .x = ttext->bs->off[1],
        .rows = size[0],
        .cols = size[1]+1,      // scrollbar
        .userptr = NULL,
        .name = "textinner",
        .resizecb = NULL,
        .flags = 0,
    };
    titem->n = ncplane_create(ttext->n, &nopts);

    titem->mscr = malloc(sizeof(struct mscroller));

    struct ncplane_options nopts1 = {
        .y = 0,
        .x = 0,
        .rows = size[0],
        .cols = size[1]-1,
        .userptr = NULL,
        .name = NULL,
        .resizecb = NULL,
        .flags = 0,
    };

    titem->mscr->y = 0;
    titem->mscr->n = ncplane_create(titem->n, &nopts1);
    titem->mscr->ndum = dummyplane_create(titem->mscr->n, text);
    titem->mscr->nbar = scrollbar_create(titem->n, titem->mscr->n);
    titem->mscr->sbar_show = false;
    titem->mscr->maxscroll = ncplane_dim_y(titem->mscr->ndum);


    mscroller_scrollto(titem->mscr, titem->mscr->y);


    return titem;
}


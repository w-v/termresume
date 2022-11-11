
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <notcurses/notcurses.h>

#include "text.h"
#include "utils.h"
#include "block.h"
#include "mscroller.h"
#include "mselector.h"

int sel = 0;

char* entries[] = {
"\n\
I have a passion for computers science and computer graphics, I studied software engineering before shifting to research. Today I'm looking to learn and grow my skills in the industry\n",

"\n\
Research Engineer | Inria | 2020-22 | 2 years\n\n\
• Research centered around 3D character animation and virtual crowds for video game and movie applications\n\
• Collaboration and technical assistance of PhDs and researchers\n\
• Lead developer of an open source 3D character animation research framework in Python, OpenGL and Qt (PyNimation, see Projects section)\n\
• Re-implementation of most of the animation processing algorithms using tensors, leading to up to a 100x speed up\n\
• Usage and promotion of software engineering tools and methods: versioning, tests, documentation, code quality, CI/CD, dependency management\n\
• Full-time management of a software development intern working on the PyNimation project\n\
\n\n\
Research Intern | Inria | 2020 | 6 months\n\n\
• Exploration of the research subject : Synthesising motion variations in Human Crowds\n\
• Bibliography, research problem definition, method implementation, conduction of user studies\n\
• Scientific publication : Perception of Motion Variations in Large-Scale Virtual Human Crowds\n\
\n\n\
Software Engineer Intern | ARIADNEXT | 2019 | 3 months\n\n\
• Conception and development of a real time remote trace browsing and visualization software for an Android app\n\
• Code and trace parsing with a Clang plugin, traces are sent from the embedded C++ application and received by a Python server, they are accessed through a React web application where available Android devices are announced via mDNS\
\n",

"\n\
Computer Science Research | Universit\u00e9 de Rennes I\n\
2019-20\n\
• Second year of Master of Science (MS)\n\
• Grade: 3.1 GPA\n\
\n\n\
Computer Software Engineering | Universit\u00e9 de Rennes I\n\
2018-19\n\
• First year of Master of Science (MS)\n\
• Grade: 4.0 GPA\n\
\n\n\
Computer Science | Universit\u00e9 de Rennes I\n\
2015-18\n\
• Bachelor of Science - BS\n\
• Grade: 4.0 GPA\
\n",

"\n\
PyNimation | 3D character animation research framework\n\
https://gitlab.inria.fr/lhoyet/pynimation\n\n\
• PyNimation is an open source framework for 3D character animation research\n\
• It enables importing and exporting animations from and to standard formats, editing animations as NumPy arrays, and provides classical algorithms as IK, retargeting, cyclification, etc.\n\
• It features an interactive and configurable OpenGL 3D viewer, as well as a Qt graphical user interface\n\
\n\n\
OpenMindedGL | Game engine university project\n\
https://gitlab.inria.fr/lhoyet/pynimation\n\n\
• As a end-of-year project for my Bachelor's Degree, we implemented a naive 3D game engine in C++ and OpenGL\n\
• It featured .obj loading, parameterized shaders, volumetric clouds, procedural terrain generation with different biomes, dynamic terrain loading with various levels of details (LOD)\n\
• I was involved in the general architecture and renderer, in the the procedural terrain generation and I was responsible for the dynamic terrain loading\n\
\n\n\
And many others on my github https://github.com/w-v\
\n",

"\n\
Python Developer\n\n\
• After being the main developer of a Python project for 2 years, Python has become my main language\n\
• My experience is at a point where I know most technical quirks and patterns\n\
• I have extensive experience with Python related tooling, project building and deployment (mypy, sphinx, pipenv, poetry, setuptools, etc.)\n\
• I am very familiar with numpy and scipy and I have notions of panda, tensorflow and pytorch\n\
\n\n\
Graphics Developer\n\n\
• Computer graphics is my first love and is always something I like to come back to\n\
• While at Inria I acquired a good understanding of the field and its research subjects\n\
• It was also a chance to work (a lot) on my 3D math\n\
• I know OpenGL well, through personal projects and being the maintainer of a 3D engine\n\
• I worked on several personal projects in C/C++\n\
\n\n\
Linux Geek\n\n\
• Linux is my second love, and working on my environment and servers my main hobby\n\
• I've been a daily user for 7 years, I've used most common distros, I'm now using Arch (btw)\n\
• I am proficient in bash scripting\n\
• I know my way around Docker, Ansible, Vagrant\n\
• I have a homelab (https://reddit.com/r/homelab)\
\n",

"\n\
Github | https://github.com/w-v\n\n\
LinkedIn | https://www.linkedin.com/in/robinadili\n\n\
Gitlab @Inria | https://gitlab.inria.fr/radili\
\n",
    NULL
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
        .name = "textouter",
        .resizecb = NULL,
        .flags = 0,
    };
    struct ncplane* n = ncplane_create(tr->tb[TCONT]->n, &nopts);

    ncplane_set_bg_alpha(n, NCALPHA_OPAQUE);
    ncplane_set_base(n," ", 0, 0);

    /* load textture */

    ttext->n = n;

    draw_box(n, NULL);

    char** e = entries;
    int nentries = sizeof(entries)/sizeof(char*)-1;
    struct textitem** ts = malloc(nentries * sizeof(struct textitem*));
    struct textitem** t = ts;
    while(*e != NULL){
        *t = create_textitem(ttext, *e);
        t++;
        e++;
    }
    ttext->widget = ts;

    struct mselector* msel = (struct mselector*) tb[TCHOS]->widget;
    mselector_reg_callback(msel, textitem_display, tr);

    return ttext;
}

void* textitem_display(void* args, int i){
    struct tres* tr = (struct tres*) args;
    struct tblock* ttext = tr->tb[TTEXT];
    struct textitem* ntitem = ((struct textitem**) ttext->widget)[i];
    struct textitem* otitem = ((struct textitem**) ttext->widget)[sel];
    unsigned int dim[2], size[2];
    
    // hide previously selected
    ncplane_move_family_bottom(otitem->n);
    
    // display newly selected
    ncplane_move_family_top(ntitem->n);

    ncplane_dim_yx(ntitem->n, &dim[0], &dim[1]);
    size[0] =  dim[0]+ttext->bs->off[0];
    size[1] = dim[1]+ttext->bs->off[1]+1;

    ncplane_resize_simple(ttext->n, size[0], size[1]);

    // remove old box
    ncplane_erase(ttext->n);

    draw_box(ttext->n, NULL);
    box_corners(tr);

    sel = i;
    return NULL;
}

struct textitem* create_textitem(struct tblock* ttext, char* text){
    
    struct textitem* titem = malloc(sizeof(textitem));
    memset(titem, 0, sizeof(textitem));

    unsigned int size[2] = {ttext->bs->size[0]-2, ttext->bs->size[1]-ttext->bs->off[1]*2};

    titem->text = malloc(strlen(text)*sizeof(char));
    strcpy(titem->text, text);
    line_break(titem->text, size[1]+1);


    struct ncplane_options nopts = {
        .y = 1,
        .x = ttext->bs->off[1],
        .rows = size[0],
        .cols = size[1]+1,      // scrollbar
        .userptr = NULL,
        .name = "textinner",
        .resizecb = NULL,
        .flags = 0,
    };
    titem->n = ncplane_create(ttext->n, &nopts);

/*     ncplane_set_bg_alpha(titem->n, NCALPHA_OPAQUE); */
/*     ncplane_set_base(titem->n," ", 0, 0); */

    titem->mscr = malloc(sizeof(struct mscroller));

    struct ncplane_options nopts1 = {
        .y = 0,
        .x = 0,
        .rows = size[0],
        .cols = size[1],
        .userptr = NULL,
        .name = "text",
        .resizecb = NULL,
        .flags = 0,
    };

    titem->mscr->y = 0;
    titem->mscr->n = ncplane_create(titem->n, &nopts1);
    titem->mscr->ndum = dummyplane_create(titem->mscr->n, titem->text);
    if(ncplane_dim_y(titem->mscr->ndum) > size[0]){
        // plane is bigger than text
        // no need to scroll
        titem->mscr->scrolling = true;
        titem->mscr->nbar = scrollbar_create(titem->n, titem->mscr->n);
    }
    else{
        titem->mscr->scrolling = false;
        titem->mscr->nbar = NULL;
        unsigned int dim[2];
        ncplane_dim_yx(titem->mscr->ndum, &dim[0], &dim[1]);
        ncplane_resize_simple(titem->mscr->n, dim[0], dim[1]);
        ncplane_resize_simple(titem->n, dim[0], dim[1]);
        /* draw_box(titem->n, NULL); */
        /* fprintf(stderr, "titem size %d %d\n", ncplane_dim_y(titem->mscr->n), ncplane_dim_x(titem->mscr->n)); */
    }
    titem->mscr->sbar_show = false;
    titem->mscr->maxscroll = ncplane_dim_y(titem->mscr->ndum);


    mscroller_scrollto(titem->mscr, titem->mscr->y);

    ncplane_move_family_bottom(titem->n);


    return titem;
}

void destroy_textitems(struct textitem** tt){
    struct textitem* t = tt[0];
    while(t != NULL){ 
        free(t->text);
        free(t->mscr);
        free(t);
    }
    free(tt);
}

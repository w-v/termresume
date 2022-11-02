#ifndef CHOS_H
#define CHOS_H

#include <notcurses/notcurses.h>
#include "tres.h"

struct tblock* create_chos(struct notcurses* nc, struct tres* tr);

typedef struct chos_args {
    struct notcurses* nc;
    struct tres* tr;
} chos_args;

void* run_chos(void* args);

typedef struct mselector {
   int sel; 
   char** items;
   int nitems;
} mselector;

#endif


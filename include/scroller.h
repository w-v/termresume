#ifndef SCROLLER_H
#define SCROLLER_H

#include <notcurses/notcurses.h>

#include "tres.h"

typedef struct scroller_args {
    struct notcurses* nc;
    struct tres* tr;
} scroller_args;

void* scroller(void *args);
struct tblock* create_scroller(struct tres* tr);

void destroy_scroller(struct tblock* tscrol);

#endif

#ifndef TEXT_H
#define TEXT_H

#include <notcurses/notcurses.h>
#include "tres.h"

struct tblock* create_text(struct notcurses* nc, struct tres* tr);

struct textitem* create_textitem(struct tblock* ttext, char* text);

typedef struct textitem {
    struct ncplane* n;
    char* text;
    unsigned int rows;
    /* unsigned int scroll; */
} textitem;


#endif


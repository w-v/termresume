#ifndef TEXT_H
#define TEXT_H

#include <notcurses/notcurses.h>
#include "tres.h"
#include "mscroller.h"

struct tblock* create_text(struct notcurses* nc, struct tres* tr);

void destroy_text(struct tblock* ttext);

struct textitem* create_textitem(struct tblock* ttext, char* text);

void* textitem_display(void* args, int i);


typedef struct textitem {
    struct ncplane* n;
    char* text;
    unsigned int rows;
    struct mscroller* mscr;
} textitem;

#endif


#ifndef PIC_H
#define PIC_H

#include <notcurses/notcurses.h>
#include "tres.h"

struct tblock* create_pic(struct notcurses* nc, struct tres* tr);
void destroy_pic(struct tblock* tpic);

#endif


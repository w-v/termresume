#ifndef CONT_H
#define CONT_H

#include <notcurses/notcurses.h>

#include "tres.h"

struct tblock* create_cont(struct notcurses* nc, struct tres* tr);

void resize_cont(struct notcurses* nc, struct tres* tr);

void center_cont(struct notcurses* nc, struct tres* tr);

void destroy_cont(struct tblock* tcont);

#endif

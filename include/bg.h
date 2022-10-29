#ifndef BG_H
#define BG_H

#include <notcurses/notcurses.h>

#include "tres.h"

struct ncplane* create_bg(struct notcurses* nc, struct tres* tr);

#endif

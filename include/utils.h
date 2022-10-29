#ifndef UTILS_H
#define UTILS_H

#include <notcurses/notcurses.h>
#include "tres.h"

struct ncplane* boxed(struct ncplane* inner, unsigned int margy, unsigned int margx);

#define S_LEFT      0x0001
#define S_RIGHT     0x0002
#define S_UP        0x0004
#define S_DOWN      0x0008

void draw_box(struct ncplane* ntarg, char* border, struct tres* tr);
void box_corners(struct tres* tr);

#endif

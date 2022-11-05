#ifndef CHOS_H
#define CHOS_H

#include <notcurses/notcurses.h>
#include "tres.h"

struct tblock* create_chos(struct notcurses* nc, struct tres* tr);

#endif


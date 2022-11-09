#ifndef MSELECTOR_H
#define MSELECTOR_H

#include <notcurses/notcurses.h>
#include "tres.h"

bool mselector_offer_kbd(struct tblock* tb, const ncinput* nc);
bool mselector_offer_mice(struct tblock* tb, const ncinput* nc);

void mselector_unselect(struct tblock* tb);
int mselector_select(struct tblock* tb, int nsel);
struct mselector* mselector_create(struct ncplane* n, struct blocksize* bs, char* items[]);
void mselector_reg_callback(struct mselector* msel, void* (*callback)(void*, int), void* args);

typedef struct mselector {
   int sel; 
   char** items;
   int nitems;
   void* (*callback)(void*, int);
   void* callback_args;
} mselector;

#endif



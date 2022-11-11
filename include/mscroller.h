#ifndef MSCROLLER_H
#define MSCROLLER_H

#include <notcurses/notcurses.h>

typedef struct mscroller {
   int y; 
   struct ncplane* n;
   struct ncplane* ndum;
   struct ncplane* nbar;
   bool sbar_show;
   int maxscroll;
   bool scrolling;
} mscroller;

void line_break(char* t, int len);

void mscroller_scrollto(struct mscroller* mscr, int y);

bool mscroller_offer_kbd(struct mscroller* mscr, const ncinput* nc);

bool mscroller_offer_mice(struct mscroller* mscr, const ncinput* nc);

struct ncplane* dummyplane_create(struct ncplane* n, char* text);

struct ncplane* scrollbar_create(struct ncplane* parent, struct ncplane* n);

void scrollbar_update(struct mscroller* mscr);

void scrollbar_update_half(struct mscroller* mscr, char** barchars);

void scrollbar_show(struct mscroller* mscr, bool show);

#endif

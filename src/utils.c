#include <notcurses/notcurses.h>
#include "block.h"
#include "utils.h"

struct ncplane*
boxed(struct ncplane* inner, unsigned int margy, unsigned int margx){
    unsigned int rows, cols;
    ncplane_dim_yx(inner, &rows, &cols);

    int y, x;
    ncplane_yx(inner, &y, &x);

    unsigned int nrows = rows+(margy+1)*2;
    unsigned int ncols = cols+(margx+1)*2;

    struct ncplane_options npopts = {
        .y = y-1,
        .x = x-1,
        .rows = nrows,
        .cols = ncols,
        .userptr = NULL,
        .name = NULL,
        .resizecb = NULL,
        .flags = NCPLANE_OPTION_AUTOGROW,
    };
    struct ncplane* parent = ncplane_parent(inner);
    struct ncplane* np = ncplane_create(parent, &npopts);

    ncplane_reparent(inner, np);
    ncplane_move_above(inner, np);

    ncplane_move_yx(inner, margy+1, margx+1);

    ncplane_cursor_move_yx(np,0,0);
    nccell ul = NCCELL_TRIVIAL_INITIALIZER, ll = NCCELL_TRIVIAL_INITIALIZER;
    nccell lr = NCCELL_TRIVIAL_INITIALIZER, ur = NCCELL_TRIVIAL_INITIALIZER;
    nccell hl = NCCELL_TRIVIAL_INITIALIZER, vl = NCCELL_TRIVIAL_INITIALIZER;
    /* nccells_load_box(n, 0, 0, &ul, &ur, &ll, &lr, &hl, &vl, "╭╮╰╯─│"); */
    nccells_load_box(np, 0, 0, &ul, &ur, &ll, &lr, &hl, &vl, "┌┐└┘─│");
    ncplane_box_sized(np, &ul, &ur, &ll, &lr, &hl, &vl, nrows, ncols, 0);

    return np;
}

void box_lines(struct ncplane* n, int lines[2][2]){
    int pos[2] = {0, 0};
    unsigned int dim[2];
    ncplane_dim_yx(n, &dim[0], &dim[1]);
    ncplane_translate(n, NULL, &pos[0], &pos[1]); 
    /* ncplane_yx(n, &pos[0], &pos[1]); */
    lines[0][0] = pos[0];
    lines[0][1] = pos[1];
    lines[1][0] = pos[0] + dim[0]-1;
    lines[1][1] = pos[1] + dim[1]-1;
}

int box_touching(int slines[2][2], int tlines[2][2]){
    // 0   NO
    // 1    TOP
    // 2    BOTTOM
    // 3    LEFT
    // 4    RIGHT
    // (of n)
    
    int ret = 0;

    for(int e=0; e<2; e++){
        if( slines[0][e] == tlines[1][e] ){
            ret+=e*2+1;
            break;
        }
        else if( slines[1][e] == tlines[0][e] ){
            ret+=e*2+2;
            break;
        }
    }

    DEBUG("s ((%d, %d), (%d, %d)) t ((%d, %d), (%d, %d))\n",slines[0][0], slines[0][1],slines[1][0], slines[1][1], tlines[0][0], tlines[0][1],tlines[1][0], tlines[1][1]);
    DEBUG("touching %s\n", (char* [5]){"NO", "TOP", "BOTTOM", "LEFT", "RIGHT"}[ret]);
    return ret;
}

int box_inters(int slines[2][2], int tlines[2][2], int touch, int corners[2][2]){
    // TOP/BOTTOM -> compare x (1)
    // LEFT/RIGHT -> compare y (0)
    touch--;
    int d = ((touch / 2)+1)%2; 
    int l = (touch / 2);
    int s = (touch % 2);

    int ret = 0;

    if(slines[0][d] >= tlines[0][d] && slines[0][d] <= tlines[1][d]){
        corners[0][d] = slines[0][d];
        corners[0][l] = slines[s][l];
        ret = 1;
    }

    if(slines[1][d] >= tlines[0][d] && slines[1][d] <= tlines[1][d]){
        corners[1][d] = slines[1][d];
        corners[1][l] = slines[s][l];
        ret = 1;
    }

    if(tlines[0][d] >= slines[0][d] && tlines[0][d] <= slines[1][d]){
        corners[0][d] = tlines[0][d];
        corners[0][l] = slines[s][l];
        ret = 1;
    }

    if(tlines[1][d] >= slines[0][d] && tlines[1][d] <= slines[1][d]){
        corners[1][d] = tlines[1][d];
        corners[1][l] = slines[s][l];
        ret = 1;
    }
    
    return ret;
}

int is_below(struct ncplane* ns, struct ncplane* nt){
    // is nt below ns ? 
    struct ncplane* n = ns;
    int ret = 0;
    while(n != NULL){
        n = ncplane_below(n);
        if(n == nt){
            ret = 1;
            break;
        }
    }
    return ret;
}

void print_hex(const char *s)
{
  while(*s)
    DEBUG("%x", *s++);
  DEBUG("%s", "\n");
}

unsigned int char2side(char c){
    unsigned int side = 0;
    switch(c){
        case '\x80':
            side |= S_LEFT | S_RIGHT;
            break;
        case '\x82':
            side |= S_UP | S_DOWN;
            break;
        case '\x8c':
            side |= S_DOWN | S_RIGHT;
            break;
        case '\x90':
            side |= S_DOWN | S_LEFT;
            break;
        case '\x94':
            side |= S_UP | S_RIGHT;
            break;
        case '\x98':
            side |= S_UP | S_LEFT;
            break;
        case '\x9c':
            side |= S_UP | S_DOWN | S_RIGHT;
            break;
        case '\xa4':
            side |= S_UP | S_DOWN | S_LEFT;
            break;
        case '\xac':
            side |= S_RIGHT | S_DOWN | S_LEFT;
            break;
        case '\xb4':
            side |= S_RIGHT | S_UP | S_LEFT;
            break;
        case '\xbc':
            side |= S_RIGHT | S_UP | S_LEFT | S_DOWN;
            break;
    }
    return side;
}

char side2char(unsigned int side){
    char c;
    switch(side){
        case S_LEFT | S_RIGHT:
            c = '\x80';
            break;
        case  S_UP | S_DOWN:
            c = '\x82';
            break;
        case S_DOWN | S_RIGHT:
            c = '\x8c';
            break;
        case S_DOWN | S_LEFT:
            c = '\x90';
            break;
        case S_UP | S_RIGHT:
            c = '\x94';
            break;
        case S_UP | S_LEFT:
            c = '\x98';
            break;
        case S_UP | S_DOWN | S_RIGHT:
            c = '\x9c';
            break;
        case S_UP | S_DOWN | S_LEFT:
            c = '\xa4';
            break;
        case S_RIGHT | S_DOWN | S_LEFT:
            c = '\xac';
            break;
        case S_RIGHT | S_UP | S_LEFT:
            c = '\xb4';
            break;
        case S_RIGHT | S_UP | S_LEFT | S_DOWN:
            c = '\xbc';
            break;
    }
    return c;
}

char char_connect(char* s, char* t){
    /*
     *  0x80 0 ─
     *  0x82 2 │
     *
     *  0x8c 12 ┌
     *  0x90 16 ┐
     *  0x94 20 └
     *  0x98 24 ┘
     *  
     *  0x9c 28 ├
     *  0xa4 36 ┤
     *  0xac 44 ┬
     *  0xb4 52 ┴
     *
     *  0xbc 60 ┼
     *
     */
    unsigned int s1, s2;
    s1 = char2side(s[2]);
    s2 = char2side(t[2]);
    return side2char(s1 | s2);
}

void box_connect(struct ncplane* ns,struct ncplane* nt, int corn[2]){
    int rels[2], relt[2];
    char* schar, * tchar;
    char rchar[4];
    int below;

    memcpy(rels, corn, 2*sizeof(int));
    ncplane_translate_abs(ns, &rels[0], &rels[1]); 
    schar = ncplane_at_yx(ns, rels[0], rels[1], NULL, NULL);

    memcpy(relt, corn, 2*sizeof(int));
    ncplane_translate_abs(nt, &relt[0], &relt[1]); 
    tchar = ncplane_at_yx(nt, relt[0], relt[1], NULL, NULL);

    memcpy(rchar, schar, 4*sizeof(char));
    rchar[2] = char_connect(schar, tchar);

    DEBUG("source char %s, len %d\n", schar, strlen(schar));
    DEBUG("target char %s, len %d\n", tchar, strlen(tchar));
    DEBUG("result char %s, len %d\n", rchar, strlen(rchar));
    
    // /!\ save and restore cursor position
    unsigned int cpos[2];

    nccell rcell = NCCELL_TRIVIAL_INITIALIZER;
    if(is_below(ns, nt)){
        DEBUG("%s\n", "target below source\n");
        DEBUG("%s\n", "printing to source\n");
        ncplane_cursor_yx(ns, &cpos[0], &cpos[1]);
        nccell_load(ns, &rcell, rchar);
        ncplane_putc_yx(ns, rels[0], rels[1], &rcell);
        ncplane_cursor_move_yx(ns, cpos[0], cpos[1]);
    }
    else {
        DEBUG("%s\n", "target above source\n");
        DEBUG("%s\n", "printing to target\n");
        ncplane_cursor_yx(nt, &cpos[0], &cpos[1]);
        nccell_load(nt, &rcell, rchar);
        ncplane_putc_yx(nt, relt[0], relt[1], &rcell);
        ncplane_cursor_move_yx(nt, cpos[0], cpos[1]);
    }

}

void box_corners(struct tres* tr){
    struct tblock** tb = tr->tb;
    struct ncplane* ns, * nt;
    int slines[2][2], tlines[2][2], corners[2][2] = {{0,0},{0,0}};
    int touch, inters;
    DEBUG("%s\n", "-------- BOX CORNERS --------");

    for(tblock_e* b = tr->blocks; *b != TEND; b++){
        if(tb[*b] != NULL){
            ns = tb[*b]->n;
            box_lines(ns, slines);
            DEBUG("source %s\n", ncplane_name(ns));
            for(tblock_e* db = b+1; *db != TEND; db++){
                if(tb[*db] != NULL){
                    nt = tb[*db]->n;
                    box_lines(nt, tlines);
                    DEBUG("target %s\n", ncplane_name(nt));

                    touch = box_touching(slines, tlines);
                    if(touch){
                        inters = box_inters(slines, tlines, touch, corners);
                        if(inters){
                            DEBUG("FOUND CORNERS ((%d, %d), (%d, %d))\n",corners[0][0], corners[0][1],corners[1][0], corners[1][1]);
                            box_connect(ns, nt, corners[0]);
                            box_connect(ns, nt, corners[1]);

                        }
                        
                    }
                }
            }
        }
    }
    DEBUG("%s\n", "-----------------------------");

}

void draw_box(struct ncplane* ntarg, char* border){
    if(border == NULL){
        border = "┌┐└┘─│";
    }
    unsigned int rows, cols;
    ncplane_dim_yx(ntarg, &rows, &cols);

    unsigned int cur_pos[2];
    ncplane_cursor_yx(ntarg,&cur_pos[0],&cur_pos[1]);
    ncplane_cursor_move_yx(ntarg,0,0);
    nccell ul = NCCELL_TRIVIAL_INITIALIZER, ll = NCCELL_TRIVIAL_INITIALIZER;
    nccell lr = NCCELL_TRIVIAL_INITIALIZER, ur = NCCELL_TRIVIAL_INITIALIZER;
    nccell hl = NCCELL_TRIVIAL_INITIALIZER, vl = NCCELL_TRIVIAL_INITIALIZER;
    nccells_load_box(ntarg, 0, 0, &ul, &ur, &ll, &lr, &hl, &vl, border);
    ncplane_box_sized(ntarg, &ul, &ur, &ll, &lr, &hl, &vl, rows, cols, 0);

    ncplane_cursor_move_yx(ntarg,cur_pos[0],cur_pos[1]);

    /* struct tblock** tb = tr->tb; */
    /* int corners[4][2] = { */ 
    /*     {0, 0}, */
    /*     {rows, 0}, */
    /*     {rows, cols}, */
    /*     {0, cols}, */
    /* }; */
    /* int corners_step[4][3] = { */ 
    /*     {0, rows, 1}, */
    /*     {1, cols, 1}, */
    /*     {0, -rows, -1}, */
    /*     {1, -cols, -1}, */
    /* }; */
    /* int rel[2], abs[2]; */
    /* unsigned int dim[2]; */
    /* int* corn, * cst; */
    /* bool within; */
    /* char* charat; */

    /* DEBUG("target %s\n", ncplane_name(ntarg)); */
    /* for(tblock_e* b = tr->blocks; *b != TEND; b++){ */

    /*     if(tb[*b] != NULL && tb[*b]->n != ntarg){ */
    /*         for(int c = 0; c  4; c++){ */
    /*             corn = corners[c]; */
    /*             memcpy(abs, corn, 2*sizeof(int)); */
    /*             DEBUG("corn %d %d\n", corn[0], corn[1]); */
    /*             ncplane_translate(ntarg, NULL, &abs[0], &abs[1]); */ 
    /*             DEBUG("abs %d %d\n", abs[0], abs[1]); */

    /*             cst = corners_step[c]; */
    /*             for(int st = 0; cst < 4; st+=cst[2]){ */


    /*             memcpy(rel, abs, 2*sizeof(int)); */
    /*             within = ncplane_translate_abs(tb[*b]->n, &rel[0], &rel[1]); */
    /*             DEBUG("rel %s %d %d within %d\n", ncplane_name(tb[*b]->n), rel[0], rel[1], within); */
    /*             if(within){ */
    /*                 ncplane_dim_yx(tb[*b]->n, &dim[0], &dim[1]); */
    /*                 charat = ncplane_at_yx(tb[*b]->n, rel[0], rel[1], NULL, NULL); */

    /*                 for(int ch = 0; ch < 6; ch++){ */
    /*                     if(*charat == tb[*b]->bd->border[ch]){ */
    /*                         DEBUG("FOUND %s %s\n", ncplane_name(tb[*b]->n), charat); */
    /*                     } */
    /*                 } */
    /*             } */
    /*         } */

    /*     } */
    /* } */

}

unsigned int maxlen(char** lines){
    unsigned int max = 0, m;
    while(*lines != NULL){
        if((m = strlen(*(lines++))) > max){
            max = m;
        }
    }
    return max;
}

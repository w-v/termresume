#include <assert.h>
#include <math.h>
#include <notcurses/notcurses.h>

#include "utils.h"
#include "block.h"
#include "tres.h"

struct tblock* tblock_init(){
    struct tblock* tb = (struct tblock*) malloc(sizeof(struct tblock));
    tb->bd = (struct blockdef*) malloc(sizeof(struct blockdef));
    struct blockalign** ba = tb->bd->align;
    for(unsigned int j=0; j<2; j++){
        ba[j] = NULL;
    }
    tb->bs = (struct blocksize*) malloc(sizeof(struct blocksize));
    tb->widget = NULL;
    return tb;
}

void tblock_destroy(struct tblock* tb){
    if(tb != NULL){
        if(tb->bd != NULL){
            /* stbuct blockalign** ba = tb->bd->align; */
            /* if(ba != NULL){ */
            /*     free(ba); */
            /* } */
            free(tb->bd);
        }
        if(tb->bs != NULL){
            free(tb->bs);
        }
        if(tb->n != NULL){
            ncplane_destroy(tb->n);
        }
        free(tb);
    }
}

void compute_size(struct ncplane* nparent, struct blockdef* bd, struct blocksize* bs){
    unsigned int psize[2];
    ncplane_dim_yx(nparent, &psize[0], &psize[1]);

    for(unsigned int c = 0; c < 2; c++){
        bs->size[c] = floor(psize[c] * bd->sizerel[c]);

        bs->size[c] = bs->size[c] > bd->sizemax[c] 
            ? bd->sizemax[c] : bs->size[c];

        bs->size[c] = bs->size[c] < bd->sizemin[c] 
            ? bd->sizemin[c] : bs->size[c];

        bs->size[c] += bs->off[c]*2;
    }
}

void compute_margin(struct ncplane* nparent, struct blockdef* bd, struct blocksize* bs){
    unsigned int pmarg[2];
    ncplane_dim_yx(nparent, &pmarg[0], &pmarg[1]);

    for(unsigned int c = 0; c < 2; c++){
        bs->marg[c] = floor(pmarg[c] * bd->margrel[c]);

        bs->marg[c] = bs->marg[c] > bd->margmax[c] 
            ? bd->margmax[c] : bs->marg[c];

        bs->marg[c] = bs->marg[c] < bd->margmin[c] 
            ? bd->margmin[c] : bs->marg[c];
    }
}

void compute_pos(struct ncplane* nparent, struct blockdef* bd, struct blocksize* bs){

    struct ncplane* base;
    ncalign_e side, bside;
    int bpos[2];
    unsigned int bsize[2];

    for(unsigned int c = 0; c < 2; c++){
        if(bd->align[c] == NULL){
            bs->pos[c] = bs->marg[c];
        }
        else{
            base = bd->align[c]->base;

            assert(nparent == ncplane_parent(base));

            side = bd->align[c]->side;
            bside = bd->align[c]->bside;

            ncplane_yx(base, &bpos[0], &bpos[1]);
            ncplane_dim_yx(base, &bsize[0], &bsize[1]);

            // align side on bside of base
            if(side == bside){
                if(bside == NCALIGN_TOP || bside == NCALIGN_LEFT){
                    bs->pos[c] = bpos[c] + bs->marg[c];
                }
                else{
                    bs->pos[c] = bpos[c] + bsize[c] - bs->size[c] + bs->marg[c];
                }
            }
            else{
                if(bside == NCALIGN_TOP || bside == NCALIGN_LEFT){
                    bs->pos[c] = bpos[c] - bs->size[c] - bs->marg[c];
                }
                else{
                    bs->pos[c] = bpos[c] + bsize[c] + bs->marg[c]-1;
                }
            }
        }
    }
}

void compute_pad(struct ncplane* nparent, struct blockdef* bd, struct blocksize* bs){

    unsigned int ppad[2];
    ncplane_dim_yx(nparent, &ppad[0], &ppad[1]);

    for(unsigned int c = 0; c < 2; c++){
        bs->off[c] = floor(ppad[c] * bd->padrel[c]);

        bs->off[c] = bs->off[c] > bd->padmax[c] 
            ? bd->padmax[c] : bs->off[c];

        bs->off[c] = bs->off[c] < bd->padmin[c] 
            ? bd->padmin[c] : bs->off[c];

        if(bd->border != NULL){
            bs->off[c]++;
        }
    }
}

void compute_block(struct ncplane* nparent, struct blockdef* bd, struct blocksize* bs){
    compute_pad(nparent,bd,bs);
    compute_size(nparent,bd,bs);
    compute_margin(nparent,bd,bs);
    compute_pos(nparent,bd,bs);
}


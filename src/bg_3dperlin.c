// generate FNL implementation
#define FNL_IMPL
#define M_PI 3.14159265358979323846

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <notcurses/notcurses.h>
#include "FastNoiseLite.h"
#include "term3d_c/tgeom.h"
#include "term3d_c/geom.h"
#include "term3d_c/fbuf.h"
#include "term3d_c/render.h"
#include "cglm/cglm.h"
#include "bg_3dperlin.h"
#include "tres.h"

void terrain_normals_ln(geom* g, int len, int height, int i){
    vec3 h = {0.f, 0.f, 1.f} , l = {1.f, 0.f, 0.f};
    int id = i*len;
    int hi[2] = {id-len, id+len}, li[2] = {id-1, id+1};
    if(i < 2){
        hi[0] = 0;
    }
    if(i == height - 1){
        hi[1] = id;
    }
    for(int j=0; j<len; j++){
        if(j < 2){
            li[0] = i*len;
        }
        if(j == len - 1){
            li[1] = id;
        }
        h[1] = g->pts[hi[1]++][1] - g->pts[hi[0]++][1];
        l[1] = g->pts[li[1]++][1] - g->pts[li[0]++][1];
        glm_vec3_cross(l, h, g->norm[id]);
        glm_vec3_normalize(g->norm[id++]);
    }
}

void genterrain_ln(geom* g, int len, int height, int x, float scale){

    // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.frequency = 0.05;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

    assert(g->npts == len*height);

    int i = (x%height);
    int index = i * len;
    float off = -len/2.f;
    for(int j=0; j<len; j++){
        /* memcpy(g->pts[index], ((vec3){i, 0.f, j}), sizeof(vec3)); */
        g->pts[index][0] = ((float)j+off)*scale;
        g->pts[index][1] = (fnlGetNoise2D(&noise, x*scale, j*scale)+1)*5;
        g->pts[index][2] = ((float)len+off)*scale;

        index++;
    }
    /* transform(g, g, NULL, (vec3){-len/2.f,0.f,-len/2.f}, (vec3){scale,scale,scale}); */

}



struct bg_3dperlin* bg_3dperlin_create(struct notcurses* nc){

    struct bg_3dperlin* bg3d = malloc(sizeof(bg_3dperlin));

    // terminal geometry
    unsigned int dim[2];
    notcurses_stddim_yx(nc, &dim[0], &dim[1]);
    /* tgeom[0]--; */
    bg3d->tgeom[0] = dim[0];
    bg3d->tgeom[1] = dim[1];
    
    // frame buffer
    // geom[1]+1 because of extra \n at each line end
    bg3d->fbuf = createfbuf(bg3d->tgeom);

    // z buffer
    bg3d->zbuf = createzbuf(bg3d->tgeom);

    bg3d->rargs = malloc(sizeof(renderargs));

    default_rargs(bg3d->rargs);
    int len = 40*8;
    bg3d->len = len;

    bg3d->g = creategeom(len*len);
    cleargeom(bg3d->g);
    bg3d->tg = creategeom(len*len);

    memcpy(bg3d->rot, (vec3) {0,0,0}, sizeof(bg3d->rot));
    memcpy(bg3d->tr, (vec3) {0,-20,len/2/4}, sizeof(bg3d->tr));
    bg3d->scale = 1.f/4;

    bg3d->rargs->lightdir[0] = -1.f;
    bg3d->rargs->lightdir[1] = -0.3f;
    bg3d->rargs->lightdir[2] = 0.f;

    glm_vec3_normalize(bg3d->rargs->lightdir);

    return bg3d;
}

void bg_3dperlin_destroy(struct bg_3dperlin* bg3d){
    destroygeom(bg3d->g);
    destroygeom(bg3d->tg);
    destroyfbuf(bg3d->fbuf, bg3d->tgeom);
    destroyzbuf(bg3d->zbuf);
    free(bg3d->rargs);
    free(bg3d);
}

void* bg_3dperlin_run(struct notcurses* nc, struct tres* tr, struct ncplane* n, struct bg_3dperlin* bg3d){

    int msdelay = 1000.f/70;
    struct timespec fdelay = { .tv_sec = 0, .tv_nsec = msdelay * 1000000};

    int i = 0, len = bg3d->len;

    clock_t t0 = clock(), tdiff =  CLOCKS_PER_SEC * 6;

    while(run){

        if(clock()-t0 > tdiff){

            nanosleep(&fdelay, NULL);
        
            clearfbuf(bg3d->fbuf, bg3d->tgeom);
            clearzbuf(bg3d->zbuf, bg3d->tgeom);

            genterrain_ln(bg3d->g, len, len, i, bg3d->scale);
            transform(bg3d->g, bg3d->g, NULL, (vec3){0,0,-bg3d->scale}, NULL);
            terrain_normals_ln(bg3d->g, len, len, (i-1)%len);

            transform(bg3d->g, bg3d->tg, bg3d->rot, bg3d->tr, NULL);

            render(bg3d->tg, bg3d->fbuf, bg3d->zbuf, bg3d->tgeom, bg3d->rargs);

            /* printfbuf(fbuf, tgeom); */
            for(int i=0; i<bg3d->tgeom[0]; i++){
                ncplane_cursor_move_yx(n, i, 0);
                ncplane_printf(n, "%s", bg3d->fbuf[i]);
            }
        }
        
        notcurses_render(nc);
        
        i++;
        /* return NULL; */
        /* nanosleep(&ts, NULL); */
    }
     
}

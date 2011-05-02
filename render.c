#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <zlib.h>
#include "editor.c"
#include "bmp.c"

typedef struct {
  int minX; int maxX;
  int minY; int maxY;
  int minZ; int maxZ;
  char init;
} ibounds;

typedef struct {
  float r; float g;
  float b; float a;
} color;

void capply(color *a, const color *b) {
  float y = b->a; float z = 1-y;
  a->r = a->r*z+b->r*y;
  a->g = a->g*z+b->g*y;
  a->b = a->b*z+b->b*y;
  a->a = 1-(1-a->a)*z;
}

void ctop(pixel *p, color *c) {
  p->r = (int)(c->r*255*c->a+0.5);
  p->g = (int)(c->g*255*c->a+0.5);
  p->b = (int)(c->b*255*c->a+0.5);
}

int bounds(chunk *r, int x, int z, void *ctx) {
  ibounds *b = ctx;
  if(b->init) {
    if(x < b->minX) b->minX = x;
    if(x > b->maxX) b->maxX = x;
    if(z < b->minZ) b->minZ = z;
    if(z > b->maxZ) b->maxZ = z;
  } else {
    b->minX = x; b->maxX = x;
    b->minZ = z; b->maxZ = z;
    b->init = 1;
  }
  return 0;
}

typedef struct {
  color colors[256*16];
  ibounds b;
  bmp *bmp;
} rctx;

int render(chunk *r, int sx, int sz, void *ctx) {
  int x, y, z;
  rctx *rc = ctx;
  color *COLORS = rc->colors;
  ibounds b = rc->b;
  bmp *bmp = rc->bmp;
  tag t; t.data = NULL;
  t.name = NULL; t.nlen = 0;
  tag_parse(&t,r);
  if(!t.data) return 0;
  tag *tblocks; tag *tdata;
  if(!tag_find(&t,&tblocks,1,"Blocks")) {
    tag_destroy(&t); return 0;
  }
  if(!tag_find(&t,&tdata,1,"Data")) {
    tag_destroy(&t); return 0;
  }
  for(z = 0; z < 16; z++) {
    for(x = 0; x < 16; x++) {
      color c; c.r = c.g = c.b = c.a = 0.0;
      unsigned char *blocks = (char*)(tblocks->data)+((x*16)+z)*128;
      unsigned char *data = (char*)(tdata->data)+((x*16)+z)*64;
      for(y = b.maxY; y > b.minY; y--) {
	if(COLORS[blocks[y]*16+getnibble(data,y)].a == 1.0) break;
      }
      for(; y <= b.maxY; y++) {
	capply(&c,COLORS+blocks[y]*16+getnibble(data,y));
      }
      ctop(bmp_get(bmp,16*(b.maxZ-sz+1)-z-1,16*(sx-b.minX)+x),&c);
    }
  }
  tag_destroy(&t);
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc < 4 || argc > 6) {
    printf("Usage %s [world] [colors] [bmp] [ [ [bottom] ] [top] ]\n",argv[0]);
    return 2;
  }
  rctx rc;
  color *COLORS = rc.colors; color *cp = COLORS; int i;
  FILE *mf = fopen(argv[2],"r"); if(!mf) return 3;
  for(i = 0; i < 256*16; i++) {
    if(!fscanf(mf,"%f\t%f\t%f\t%f\n",&(cp->r),&(cp->g),&(cp->b),&(cp->a))) {
      printf("Failed to read colors file\n");
      return 3;
    }
    cp++;
  }
  fclose(mf);
  world *w = world_open(argv[1]);
  if(!w) {
    printf("Failed to open world\n");
    return 4;
  }
  ibounds b; bzero(&b,sizeof(b));
  world_foreach(w,&b,bounds);
  b.minY = 0; b.maxY = 127;
  if(argc == 5) {
    b.maxY = atoi(argv[4]);
    if(b.maxY > 127) b.maxY = 127;
  } else if(argc == 6) {
    b.minY = atoi(argv[4]);
    if(b.minY < 0) b.minY = 0;
    b.maxY = atoi(argv[5]);
    if(b.maxY > 127) b.maxY = 127;
  }
  rc.b = b; rc.bmp = bmp_create(16*(b.maxZ-b.minZ+1),16*(b.maxX-b.minX+1));
  world_foreach(w,&rc,render);
  FILE *bf = fopen(argv[3],"w");
  bmp_save(bf,rc.bmp);
  bmp_destroy(rc.bmp);
  fclose(bf);
}

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
  ibounds b;
  bmp *bmp;
} dctx;

int depth(chunk *r, int sx, int sz, void *ctx) {
  int x, y, z;
  dctx *dc = ctx;
  ibounds b = dc->b;
  bmp *bmp = dc->bmp;
  tag t; t.data = NULL;
  t.name = NULL; t.nlen = 0;
  tag_parse(&t,r);
  if(!t.data) return 0;
  tag *tblocks; tag *tdata;
  if(!tag_find(&t,&tblocks,1,"Blocks")) {
    tag_destroy(&t); return 0;
  }
  for(z = 0; z < 16; z++) {
    for(x = 0; x < 16; x++) {
      unsigned char *blocks = (char*)(tblocks->data)+((x*16)+z)*128;
      int g = 1;
      for(y = 127; g && y >= 0; y--) {
	switch(blocks[y]) {
	case 0:
	case 8:
	case 9:
	case 78:
	  break;
	default:
	  g = 0;
	}
      }
      if(y < 0) y = 0;
      color c; c.a = 1.0; c.r = c.g = c.b = 1.0-2.0*y;
      ctop(bmp_get(bmp,16*(b.maxZ-sz+1)-z-1,16*(sx-b.minX)+x),&c);
    }
  }
  tag_destroy(&t);
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage %s [world] [bmp]\n",argv[0]);
    return 2;
  }
  dctx dc;
  world *w = world_open(argv[1]);
  if(!w) {
    printf("Failed to open world\n");
    return 4;
  }
  ibounds b; bzero(&b,sizeof(b));
  world_foreach(w,&b,bounds);
  dc.b = b; dc.bmp = bmp_create(16*(b.maxZ-b.minZ+1),16*(b.maxX-b.minX+1));
  world_foreach(w,&dc,depth);
  FILE *bf = fopen(argv[2],"w");
  if(!bf) {
    printf("Failed to open bmp\n");
    return 5;
  }
  bmp_save(bf,dc.bmp);
  bmp_destroy(dc.bmp);
  fclose(bf);
}

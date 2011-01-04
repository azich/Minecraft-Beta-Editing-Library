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

int bounds(ibounds *b, const char *wpath) {
  int x; int z; DIR *d; int f = 0;
  char path[256]; char *ppath;
  char *ptr; char *pptr;
  strncpy(path,wpath,256);
  ppath = index(path,0);
  *(ppath++) = '/';
  struct dirent *dp; int sx; int sz;
  for(z = 0; z < 64; z++) {
    for(x = 0; x < 64; x++) {
      mkb64(ppath,x,z,"/");
      if((d = opendir(path)) == NULL) continue;
      while((dp = readdir(d)) != NULL) {
	if(dp->d_name[0] == 'c') {
	  ptr = dp->d_name+2; pptr = ptr;
	  strsep(&pptr,"."); sx = dbase36(ptr);
	  ptr = pptr; strsep(&pptr,"."); sz = dbase36(ptr);
	  if(f) {
	    if(sx < b->minX) b->minX = sx;
	    if(sx > b->maxX) b->maxX = sx;
	    if(sz < b->minZ) b->minZ = sz;
	    if(sz > b->maxZ) b->maxZ = sz;
	  } else {
	    b->minX = sx; b->maxX = sx;
	    b->minZ = sz; b->maxZ = sz;
	    f = 1;
	  }
	}
      }
      closedir(d);
    }
  }
  return f;
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage %s [world] [bmp]\n",argv[0]);
    return 2;
  }
  char path[256]; char *ppath;
  strncpy(path,argv[1],256);
  ppath = index(path,0);
  *(ppath++) = '/';
  ibounds b; if(!bounds(&b,path)) return 4;
  bmp *bmp = bmp_create(16*(b.maxZ-b.minZ+1),16*(b.maxX-b.minX+1));
  int sz; int sx; int x; int y; int z;
  for(sz = b.minZ; sz <= b.maxZ; sz++) {
    for(sx = b.minX; sx <= b.maxX; sx++) {
      mkpath(ppath,sx,sz);
      gzFile *f = gzopen(path,"r");
      if(!f) continue;
      tag t; t.data = NULL;
      t.name = NULL; t.nlen = 0;
      tag_parse(&t,f);
      gzclose(f);
      if(!t.data) continue;
      tag *tblocks;
      if(!tag_find(&t,&tblocks,1,6,"Blocks")) {
	tag_destroy(&t); continue;
      }
      char *blocks = (char*)(tblocks->data);
      for(z = 0; z < 16; z++) {
	for(x = 0; x < 16; x++) {
	  int g = 1;
	  for(y = 127; g && y >= 0; y--) {
	    switch(blocks[((x*16)+z)*128+y]) {
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
    }
  }
  FILE *bf = fopen(argv[2],"w");
  bmp_save(bf,bmp);
  bmp_destroy(bmp);
  fclose(bf);
}

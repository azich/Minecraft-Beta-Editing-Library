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
  if(argc < 4 || argc > 6) {
    printf("Usage %s [world] [colors] [bmp] [ [ [bottom] ] [top] ]\n",argv[0]);
    return 2;
  }
  color COLORS[256*16]; color *cp = COLORS; int i;
  FILE *mf = fopen(argv[2],"r"); if(!mf) return 3;
  for(i = 0; i < 256*16; i++) {
    if(!fscanf(mf,"%f\t%f\t%f\t%f\n",&(cp->r),&(cp->g),&(cp->b),&(cp->a))) {
      printf("Failed to read colors file\n");
      return 3;
    }
    cp++;
  }
  fclose(mf);
  char path[256]; char *ppath;
  strncpy(path,argv[1],256);
  ppath = index(path,0);
  *(ppath++) = '/';
  ibounds b; if(!bounds(&b,path)) return 4;
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
      tag *tblocks; tag *tdata;
      if(!tag_find(&t,&tblocks,1,6,"Blocks")) {
	tag_destroy(&t); continue;
      }
      if(!tag_find(&t,&tdata,1,4,"Data")) {
	tag_destroy(&t); continue;
      }
      for(z = 0; z < 16; z++) {
	for(x = 0; x < 16; x++) {
	  color c; c.r = c.g = c.b = c.a = 0.0;
	  unsigned char *blocks = (char*)(tblocks->data)+((x*16)+z)*128;
	  unsigned char *data = (char*)(tdata->data)+((x*16)+z)*64;
	  for(y = b.maxY; y >= b.minY; y--) {
	    if(COLORS[blocks[y]*16+getnibble(data,y)].a == 1.0) break;
	  }
	  for(; y <= b.maxY; y++) {
	    capply(&c,COLORS+blocks[y]*16+getnibble(data,y));
	  }
	  ctop(bmp_get(bmp,16*(b.maxZ-sz+1)-z-1,16*(sx-b.minX)+x),&c);
	}
      }
      tag_destroy(&t);
    }
  }
  FILE *bf = fopen(argv[3],"w");
  bmp_save(bf,bmp);
  bmp_destroy(bmp);
  fclose(bf);
}

#include "editor.c"
#include <strings.h>

int ttile_pos(int **pos, tag *t) {
  tag *tpos[3];
  if(tag_find(t,tpos+0,1,1,"x") && tpos[0]->type == 3) {
    if(tag_find(t,tpos+1,1,1,"y") && tpos[1]->type == 3) {
      if(tag_find(t,tpos+2,1,1,"z") && tpos[2]->type == 3) {
	pos[0] = tpos[0]->data;
	pos[1] = tpos[1]->data;
	pos[2] = tpos[2]->data;
	return 1;
      }
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [world]\n",argv[0]);
    return 2;
  }
  int x; int y; int z; DIR *d;
  char path[256]; char *ppath;
  char *ptr; char *pptr;
  strncpy(path,argv[1],256);
  ppath = index(path,0);
  *(ppath++) = '/';
  struct dirent *dp; int sx; int sz;
  for(sz = 0; sz < 64; sz++) {
    for(sx = 0; sx < 64; sx++) {
      mkb64(ppath,sx,sz,"/");
      char *dpath = index(path,0);
      *(dpath++) = '/'; *dpath = 0;
      if((d = opendir(path)) == NULL) continue;
      while((dp = readdir(d)) != NULL) {
        if(dp->d_name[0] == 'c') {
          strcpy(dpath,dp->d_name);
          ptr = dp->d_name+2; pptr = ptr;
          strsep(&pptr,"."); x = dbase36(ptr);
          ptr = pptr; strsep(&pptr,"."); z = dbase36(ptr);
          gzFile *f = gzopen(path,"r");
          tag t; t.nlen = 0; t.name = NULL;
	  t.data = NULL; tag_parse(&t,f);
          gzclose(f);
	  if(t.data == NULL) continue;
	  tag *tents;
	  if(tag_find(&t,&tents,1,12,"TileEntities") && tents->type == 9) {
	    tag *ents = (tag*)tents->data; tag *tid; tag *ttext; int i;
	    for(i = 0; i < tents->length; i++) {
	      if(tag_find(ents+i,&tid,1,2,"id") && tid->type == 8) {
		if(tid->length == 4 && strncmp((char*)tid->data,"Sign",4) == 0) {
		  int *pos[3]; tag *text[4];
		  ttile_pos(pos,ents+i);
		  if(!tag_find(ents+i,text+0,1,5,"Text1")) continue;
		  if(!tag_find(ents+i,text+1,1,5,"Text2")) continue;
		  if(!tag_find(ents+i,text+2,1,5,"Text3")) continue;
		  if(!tag_find(ents+i,text+3,1,5,"Text4")) continue;
		  printf("(%i+%i,%i,%i+%i)\n",x,*pos[0],*pos[1],z,*pos[2]);
		  if(text[0]->data != NULL) { printf("\"%s\"\n",(char*)text[0]->data); } else { printf("\"\"\n"); }
		  if(text[1]->data != NULL) { printf("\"%s\"\n",(char*)text[1]->data); } else { printf("\"\"\n"); }
		  if(text[2]->data != NULL) { printf("\"%s\"\n",(char*)text[2]->data); } else { printf("\"\"\n"); }
		  if(text[3]->data != NULL) { printf("\"%s\"\n",(char*)text[3]->data); } else { printf("\"\"\n"); }
		  printf("\n");
		}
	      }
	    }
	  }
	  tag_destroy(&t);
	}
      }
      closedir(d);
    }
  }
}

#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc < 2 || argc > 4) {
    printf("Usage: %s [world] [ [delay] [ [type] ] ]\n",argv[0]);
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
	  tag *tents; int changed = 0;
	  if(tag_find(&t,&tents,1,12,"TileEntities") && tents->type == 9) {
	    tag *ents = (tag*)tents->data; int i;
	    for(i = 0; i < tents->length; i++) {
	      tag *ttag;
	      if(!tag_find(ents+i,&ttag,1,2,"id") || ttag->type != 8) continue;
	      if(ttag->length != 10 || strncmp(ttag->data,"MobSpawner",10) != 0) continue;
	      if(!tag_find(ents+i,&ttag,1,1,"x") || ttag->type != 3) continue;
	      x = *(int*)ttag->data;
	      if(!tag_find(ents+i,&ttag,1,1,"y") || ttag->type != 3) continue;
	      y = *(int*)ttag->data;
              if(!tag_find(ents+i,&ttag,1,1,"z") || ttag->type != 3) continue;
	      z = *(int*)ttag->data;
	      if(!tag_find(ents+i,&ttag,1,8,"EntityId") || ttag->type != 8) continue;
              if(argc >= 4) {
		changed = 1; free(ttag->data);
		ttag->length = strlen(argv[3]);
		ttag->data = malloc(ttag->length+1);
		strcpy(ttag->data,argv[3]);
	      }
	      char *id = (char*)ttag->data;
	      if(!tag_find(ents+i,&ttag,1,5,"Delay") || ttag->type != 2) continue;
	      if(argc >= 3) {
		*(short*)ttag->data = atoi(argv[2]);
              }
	      short delay = *(short*)ttag->data;
	      printf("(%i,%i) + (%i,%i,%i) = %s (%i)\n",sx,sz,x,y,z,id,delay);
	    }
	  }
	  if(changed) {
	    f = gzopen(path,"w");
	    tag_serial(&t,f);
	    gzclose(f);
	  }
	  tag_destroy(&t);
	}
      }
      closedir(d);
    }
  }
}

#include "editor.c"

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
	  tag *results; int bid;
	  if(tag_find(&t,&results,1,6,"Blocks") && results->type == 7) {
	    char *blocks = (char*)results->data;
	    int x; int y; int z;
	    for(x = 0; x < 16; x++) {
	      for(z = 0; z < 16; z++) {
		for(y = 0; y < 128; y++) {
		  int bid = ((x*16)+z)*128+y;
		  switch(blocks[bid]) {
		  case 8:
		    blocks[bid] = 10;
		    break;
		  case 9:
		  case 79:
		    blocks[bid] = 11;
		  }
		}
	      }
	    }
	    gzFile *f = gzopen(path,"w");
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

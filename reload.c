#include "editor.c"

void tag_name(tag *t, char *s) {
  t->nlen = strlen(s);
  t->name = malloc(t->nlen+1);
  strncpy(t->name,s,t->nlen+1);
}

void new_item(tag *t, short id, short damage, char count, char slot) {
  tag *kids = malloc(4*sizeof(tag));
  short *mid = malloc(sizeof(short)); *mid = id;
  kids[0].data = mid; kids[0].type = 2; tag_name(kids+0,"id");
  short *mdamage = malloc(sizeof(short)); *mdamage = damage;
  kids[1].data = mdamage; kids[1].type = 2; tag_name(kids+1,"Damage");
  char *mcount = malloc(sizeof(char)); *mcount = count;
  kids[2].data = mcount; kids[2].type = 1; tag_name(kids+2,"Count");
  char *mslot = malloc(sizeof(char)); *mslot = slot;
  kids[3].data = mslot; kids[3].type = 1; tag_name(kids+3,"Slot");
  t->type = 10; t->data = kids; t->length = 4;
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage: %s [world] [id]\n",argv[0]);
    return 2;
  }
  int x; int y; int z; DIR *d;
  char path[256]; char *ppath;
  char *ptr; char *pptr;
  strncpy(path,argv[1],256);
  short tid = atoi(argv[2]);
  ppath = index(path,0);
  *(ppath++) = '/';
  int dispensers = 0; long long arrows = 0;
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
	      if(ttag->length != 4 || strncmp(ttag->data,"Trap",4) != 0) continue;
	      tag *items;
	      if(!tag_find(ents+i,&items,1,5,"Items") || items->type != 9) continue;
	      int reload = 1; int h;
	      for(h = 0; h < items->length; h++) {
		if(tag_find((tag*)items->data+h,&ttag,1,2,"id")) {
		  if(*(short*)ttag->data != tid) {
		    reload = 0; break;
		  }
		}
	      }
	      if(!reload) continue;
	      changed = 1; dispensers++;
	      for(h = 0; h < items->length; h++) {
		if(tag_find((tag*)items->data+h,&ttag,1,5,"Count")) {
		  arrows -= *(unsigned char*)ttag->data;
		}
		tag_destroy((tag*)items->data+h);
	      }
	      items->data = realloc(items->data,sizeof(tag)*9);
	      bzero(items->data,sizeof(tag)*9);
	      items->length = 9;
	      for(h = 0; h < items->length; h++) {
		arrows += 64;
		new_item((tag*)items->data+h,tid,0,64,h);
	      }
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
  printf("Found %i dispensers(s) for item %i. Loaded %llu projectile(s).\n",dispensers,tid,arrows);
}

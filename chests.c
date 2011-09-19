#include "editor.c"
#include <strings.h>

int ttile_pos(int **pos, tag *t) {
  tag *tpos[3];
  if(tag_find(t,tpos+0,1,"x") && tpos[0]->type == 3) {
    if(tag_find(t,tpos+1,1,"y") && tpos[1]->type == 3) {
      if(tag_find(t,tpos+2,1,"z") && tpos[2]->type == 3) {
	pos[0] = tpos[0]->data;
	pos[1] = tpos[1]->data;
	pos[2] = tpos[2]->data;
	return 1;
      }
    }
  }
  return 0;
}

int chests(chunk *r, int x, int z, void *ctx) {
  tag t; t.data = NULL; tag_parse(&t,r);
  if(t.data == NULL) return 0;
  tag *tents;
  if(tag_find(&t,&tents,1,"TileEntities") && tents->type == 9) {
    tag *ents = (tag*)tents->data; tag *tid; tag *ttext; int i;
    for(i = 0; i < tents->length; i++) {
      if(tag_find(ents+i,&tid,1,"id") && tid->type == 8) {
	if(tid->length == 5 && strncmp((char*)tid->data,"Chest",5) == 0) {
	  int *pos[3]; tag *items;
	  ttile_pos(pos,ents+i);
	  if(!tag_find(ents+i,&items,1,"Items") || items->type != 9) continue;
	  printf("(%i+%i,%i,%i+%i)\n",x,*pos[0],*pos[1],z,*pos[2]);
	  unsigned int ii = 0;
	  while(ii < items->length) {
	    tag *itag;
	    if(tag_find((tag*)items->data+ii,&itag,1,"id")) {
	      printf("\tID: %i",*(short*)itag->data);
	    }
	    if(tag_find((tag*)items->data+ii,&itag,1,"Damage")) {
	      printf("\tDamage: %i",*(short*)itag->data);
	    }
	    if(tag_find((tag*)items->data+ii,&itag,1,"Count")) {
	      printf("\tCount: %i",*(char*)itag->data);
	    }
	    if(tag_find((tag*)items->data+ii,&itag,1,"Slot")) {
	      printf("\tSlot: %i",*(char*)itag->data);
	    }
	    printf("\n"); ii++;
	  }
	  printf("\n");
	}
      }
    }
  }
  tag_destroy(&t);
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [world]\n",argv[0]);
    return 2;
  }
  world *w = world_open(argv[1]);
  if(!w) {
    printf("Failed to open world\n");
    return 2;
  }
  world_foreach(w,NULL,chests);
  world_close(w);
}

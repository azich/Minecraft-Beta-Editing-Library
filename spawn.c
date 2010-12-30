#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 2 && argc != 5) {
    printf("Usage: %s [level.dat] [ [x] [y] [z] ]\n",argv[0]);
    return 2;
  }
  gzFile *f = gzopen(argv[1],"r");
  if(!f) {
    printf("Cound not open file\n");
    return 3;
  }
  tag t; t.data = NULL;
  tag_parse(&t,f);
  gzclose(f);
  if(!t.data) {
    printf("No data found\n");
    return 5;
  }
  int x,y,z; tag *ttpos;
  if(argc > 2) {
    x = atoi(argv[2]);
    y = atoi(argv[3]);
    z = atoi(argv[4]);
  } else {
    if(!tag_find(&t,&ttpos,1,3,"Pos") || ttpos->type != 9) {
      printf("Could not find Pos tag\n");
      tag_destroy(&t); return 6;
    }
    tag *tpos = (tag*)ttpos->data;
    x = (int)(*(double*)tpos[0].data+0.5);
    y = (int)(*(double*)tpos[1].data+0.5);
    z = (int)(*(double*)tpos[1].data+0.5);
  }
  tag *tspawn[3];
  if(!tag_find(&t,tspawn+0,1,6,"SpawnX") || tspawn[0]->type != 3) {
    printf("Could not find SpawnX tag\n");
    tag_destroy(&t); return 6;
  }
  if(!tag_find(&t,tspawn+1,1,6,"SpawnY") || tspawn[1]->type != 3) {
    printf("Could not find SpawnY tag\n");
    tag_destroy(&t); return 6;
  }
  if(!tag_find(&t,tspawn+2,1,6,"SpawnZ") || tspawn[2]->type != 3) {
    printf("Could not find SpawnZ tag\n");
    tag_destroy(&t); return 6;
  }
  *(int*)tspawn[0]->data = x;
  *(int*)tspawn[1]->data = y;
  *(int*)tspawn[2]->data = z;
  f = gzopen(argv[1],"w");
  tag_serial(&t,f); gzclose(f);
  tag_destroy(&t);
}

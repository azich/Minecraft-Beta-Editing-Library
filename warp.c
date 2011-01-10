#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 5) {
    printf("Usage: %s [level.dat] [x] [y] [z]\n",argv[0]);
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
  double x, y, z; tag *ttpos;
  x = atof(argv[2]);
  y = atof(argv[3]);
  z = atof(argv[4]);
  if(!tag_find(&t,&ttpos,1,3,"Pos") || ttpos->type != 9) {
    printf("Could not find Pos tag\n");
    tag_destroy(&t); return 6;
  }
  tag *tpos = (tag*)ttpos->data;
  *(double*)tpos[0].data = x;
  *(double*)tpos[1].data = y;
  *(double*)tpos[2].data = z;
  f = gzopen(argv[1],"w");
  tag_serial(&t,f); gzclose(f);
  tag_destroy(&t);
}

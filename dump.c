#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [world]\n",argv[0]);
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
  tag_tree(&t);
  tag_destroy(&t);
}

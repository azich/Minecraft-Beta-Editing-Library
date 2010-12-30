#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage: %s [level.dat] [time]\n",argv[0]);
    return 2;
  }
  long long ntime = strtoll(argv[1],(char**)NULL,10);
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
  tag *results;
  if(!tag_find(&t,&results,1,4,"Time") || results->type != 4) {
    printf("Could not find Time tag\n");
    tag_destroy(&t); return 6;
  }
  *(long long*)results->data = ntime;
  f = gzopen(argv[1],"w");
  tag_serial(&t,f); gzclose(f);
  tag_destroy(&t);
}

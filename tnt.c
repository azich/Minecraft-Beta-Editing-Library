#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [chunk]\n",argv[0]);
    return 2;
  }
  gzFile *f = gzopen(argv[1],"r");
  if(!f) {
    printf("Cound not open file\n");
    return 3;
  }
  tag t; t.data = NULL;
  tag_parse(&t,f);
  if(!t.data) {
    printf("No data found\n");
    gzclose(f); return 5;
  }
  gzclose(f);
  tag *tblocks;
  if(!tag_find(&t,&tblocks,1,6,"Blocks")) {
    printf("No blocks found\n");
    tag_destroy(&t); return 6;
  }
  tag_tree(&t);
  char *blocks = (char*)tblocks->data;
  int x; int y; int z;
  for(x = 0; x < 16; x++) {
    for(z = 0; z < 16; z++) {
      for(y = 0; y < 128; y++) {
	int bid = ((x*16)+z)*128+y;
	if(y <= 63) {
	  blocks[bid] = 7;
	} else {
	  if(y % 2 == 0) {
	    if(x % 2 == 0) {
	      blocks[bid] = 46;
	    } else {
	      blocks[bid] = 3;
	    }
	  } else {
	    blocks[bid] = 55;
	  }
	}
      }
    }
  }
  tag_tree(&t);
  f = gzopen(argv[1],"w");
  if(!f) {
    printf("Failed to write file\n");
    tag_destroy(&t); return 7;
  }
  tag_serial(&t,f);
  gzclose(f);
  tag_destroy(&t);
}

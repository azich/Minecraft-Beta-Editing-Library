#include "editor.c"

int main(int argc, char *argv[]) {
  if(argc != 5) {
    printf("Usage: %s [world] [x] [z] [NESW]\n",argv[0]);
    return 2;
  }
  char path[512]; char *ppath = stpcpy(path,argv[1]); *(ppath++) = '/';
  int x = atoi(argv[2]); int y = 0; int z = atoi(argv[3]); char d = argv[4][0];
  int sx = (x < 0 ? (x+1)/16-1 : x/16); x = (x < 0 ? (x+1)%16+15 : x%16);
  int sz = (z < 0 ? (z+1)/16-1 : z/16); z = (z < 0 ? (z+1)%16+15 : z%16);
  switch(d) {
  case 'n':
  case 'N':
    d = 0x05;
    break;
  case 'e':
  case 'E':
    d = 0x03;
    break;
  case 'w':
  case 'W':
    d = 0x02;
    break;
  case 's':
  case 'S':
    d = 0x04;
    break;
  default:
    printf("Please specify NESW\n");
    return 3;
  }
  mkpath(ppath,sx,sz);
  gzFile *f = gzopen(path,"r");
  if(!f) {
    printf("Cound not open file\n");
    return 4;
  }
  tag t; t.data = NULL;
  tag_parse(&t,f);
  gzclose(f);
  if(!t.data) {
    printf("No data found\n");
    return 5;
  }
  tag *tblocks; tag *tdata;
  if(tag_find(&t,&tblocks,1,6,"Blocks")) {
    if(tag_find(&t,&tdata,1,4,"Data")) {
      char *blocks = tblocks->data+((x*16)+z)*128;
      char *data = tdata->data+((x*16)+z)*128/2;
      for(y = 127; y >= 0; y--) {
	if(blocks[y] != 0) break;
      }
      for(y++; y < 128; y++) {
	data[y/2] = y&1?(data[y/2]&0xf0)|d:(data[y/2]&0x0f)|(d<<4);
	blocks[y] = 65;
      }
      f = gzopen(path,"w");
      tag_serial(&t,f);
      gzclose(f);
      printf("Done\n");
    } else {
      printf("Data not found\n");
    }
  } else {
    printf("Blocks not found\n");
  }
  tag_destroy(&t);
}

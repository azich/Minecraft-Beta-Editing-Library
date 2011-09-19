#include <fcntl.h>
#include <sys/mman.h>
#include "editor.c"

int counter(chunk *r, int sx, int sz, void *ctx) {
  long long *count = (long long*)ctx;
  tag t; t.nlen = 0; t.name = NULL;
  t.data = NULL; tag_parse(&t,r);
  if(t.data == NULL) return 0;
  tag *tblocks; tag *tdata;
  if(tag_find(&t,&tblocks,1,"Blocks") && tblocks->type == 7) {
    if(tag_find(&t,&tdata,1,"Data") && tdata->type == 7) {
      int x; int y; int z;
      for(x = 0; x < 16; x++) {
	for(z = 0; z < 16; z++) {
	  char *blocks = tblocks->data;
	  char *data = tblocks->data;
	  blocks += ((x*16)+z)*128;
	  data += ((x*16)+z)*64;
	  for(y = 0; y < 128; y++) {
	    count[(y*256+blocks[y])*16+getnibble(data,y)]++;
	  }
	}
      }
    }
  }
  tag_destroy(&t);
  return 0;
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    printf("Usage: %s [world] [counter.dat]\n",argv[0]);
    return 2;
  }
  int fd, exists = 0;
  if((fd = open(argv[2],O_RDONLY,0644)) >= 0) {
    exists = 1; close(fd);
  }
  if((fd = open(argv[2],O_RDWR|O_CREAT,0644)) < 0) {
    printf("Failed to open %s\n",argv[2]);
    return 3;
  }
  lseek(fd,128*256*16*sizeof(long long)-1,SEEK_SET);
  write(fd,"",1);
  lseek(fd,0,SEEK_SET);
  long long *count = mmap(0, 128*256*16*sizeof(long long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(count == (long long*) -1) {
    printf("Failed to mmap\n");
    return 4;
  }
  if(!exists) bzero(count,128*256*16*sizeof(long long));
  world *w = world_open(argv[1]);
  if(!w) {
    printf("Failed to open world\n");
    return 5;
  }
  world_foreach(w,count,counter);
  close(fd);
}

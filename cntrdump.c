#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Usage: %s [counter.dat]\n",argv[0]);
    return 2;
  }
  int fd;
  if((fd = open(argv[1],O_RDONLY,0644)) < 0) {
    printf("Failed to open %s\n",argv[1]);
    return 3;
  }
  if(lseek(fd,0,SEEK_END) != 128*256*16*sizeof(long long)) {
    printf("Wrong size for %s\n",argv[1]);
    return 4;
  }
  lseek(fd,0,SEEK_SET);
  long long *count = mmap(0, 128*256*16*sizeof(long long), PROT_READ, MAP_SHARED, fd, 0);
  if(count == (long long*) -1) {
    printf("Failed to mmap\n");
    return 4;
  }
  printf("create table if not exists blocks (layer int, block int, data int, n int);\n");
  printf("begin transaction;\n");
  int y; int i; int d;
  for(y = 0; y < 128; y++) {
    for(i = 0; i < 256; i++) {
      for(d = 0; d < 16; d++) {
	printf("insert into blocks (layer, block, data, n) values (%i, %i, %i, %llu);\n",y,i,d,count[(y*256+i)*16+d]);
      }
    }
  }
  printf("end transaction;\n");
  close(fd);
}

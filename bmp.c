#define BUFFER 65536

typedef struct {
  unsigned int wb;
  unsigned int w;
  signed int h;
  char *data;
} bmp;

typedef struct {
  unsigned char b;
  unsigned char g;
  unsigned char r;
} pixel;

void bmp_load(bmp *b, FILE *f);
bmp* bmp_create(unsigned int w, signed int h);
void bmp_save(FILE *f, bmp *b);
void bmp_destroy(bmp *b);
pixel* bmp_get(bmp *b, unsigned int x, unsigned int y);

void bmp_load(bmp *b, FILE *f) {
  fseek(f,18,SEEK_SET);
  fread(&(b->w),4,1,f);
  fread(&(b->h),4,1,f);
  b->wb = 3*b->w;
  b->wb += (4-(b->wb%4))%4;
  b->data = malloc(b->wb*b->h);
  fseek(f,54,SEEK_SET);
  fread(b->data,1,b->wb*b->h,f);
}

bmp* bmp_create(unsigned int w, signed int h) {
  bmp *b = malloc(sizeof(bmp));
  b->w = w; b->h = h; b->wb = 3*w+(4-((3*w)%4))%4;
  b->data = malloc(b->wb*b->h); bzero(b->data,b->wb*b->h);
  return b;
}

void bmp_save(FILE *f, bmp *b) {
  char buffer[54]; bzero(buffer,54);
  int bs = (b->h<0?-b->h:b->h)*b->wb;
  int fs = bs+54;
  buffer[0] = 'B'; buffer[1] = 'M';
  buffer[10] = 54; buffer[14] = 40;
  buffer[26] = 1;  buffer[28] = 24;
  memcpy(buffer+2,&fs,4);
  memcpy(buffer+18,&(b->w),4);
  memcpy(buffer+22,&(b->h),4);
  memcpy(buffer+34,&bs,4);
  fseek(f,0,SEEK_SET);
  fwrite(buffer,1,54,f);
  fwrite(b->data,1,bs,f);
}

void bmp_destroy(bmp *b) {
  if(b != NULL) {
    if(b->data != NULL) free(b->data);
    free(b);
  }
}

pixel* bmp_get(bmp *b, unsigned int x, unsigned int y) {
  if(b->h < 0) {
    return (pixel*)(b->data+b->wb*y+3*x);
  }
  return (pixel*)(b->data+b->wb*(b->h-y-1)+3*x);
}

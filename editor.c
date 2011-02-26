#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>
#include <zlib.h>

typedef struct region region;

struct region {
  z_stream strm;
};

typedef struct tag tag;

struct tag {
  char *name;
  unsigned short nlen;
  tag *parent;
  unsigned char type;
  size_t length;
  void *data;
};

const char B36[] = "0123456789abcdefghijklmnopqrstuvwxyz";

unsigned long long htonll(unsigned long long hll);
unsigned long long ntohll(unsigned long long hll);
char getnibble(char *data, int n);
void setnibble(char *data, int n, char c);
int ebase36(char *buf, int n);
int dbase36(const char *buf);
char* mkpath(char *p, int x, int z);
char* mkb64(char *p, int x, int z, const char *d);
size_t region_read(region* r, char *d, size_t n);
size_t region_write(region* r, char *d, size_t n);
void tag_destroy(tag *t);
void tag_read(tag *t, region *r);
void tag_write(tag *t, region *r);
void tag_parse(tag *t, region *r);
void tag_serial(tag *t, region *r);
char* tag_str(tag *t, char *s, size_t n);
int tag_find(tag *t, tag **res, int count, short nlen, char *name);
void tag_tree(tag *t);
void tag_tree_(tag *t, int n);

unsigned long long htonll(unsigned long long hll) {
  union {
    unsigned long long x;
    struct {
      unsigned int a;
      unsigned int b;
    } y;
  } x;
  x.x = hll;
  x.y.a = htonl(x.y.a);
  x.y.b = htonl(x.y.b);
  if(x.x != hll) {
    unsigned int t;
    t = x.y.a;
    x.y.a = x.y.b;
    x.y.b = t;
  }
  return x.x;
}

unsigned long long ntohll(unsigned long long hll) {
  union {
    unsigned long long x;
    struct {
      unsigned int a;
      unsigned int b;
    } y;
  } x;
  x.x = hll;
  x.y.a = ntohl(x.y.a);
  x.y.b = ntohl(x.y.b);
  if(x.x != hll) {
    unsigned int t;
    t = x.y.a;
    x.y.a = x.y.b;
    x.y.b = t;
  }
  return x.x;
}

char getnibble(char *data, int n) {
  if(n % 2 == 0) {
    return data[n/2] & 0x0f;
  } else {
    return (data[n/2] & 0xf0) >> 4;
  }
}

void setnibble(char *data, int n, char c) {
  if(n % 2 == 0) {
    data[n/2] &= 0xf0;
    data[n/2] |= c&0x0f;
  } else {
    data[n/2] &= 0x0f;
    data[n/2] |= c<<4;
  }
}

int ebase36(char *buf, int n) {
  if(n < 0) {
    *buf = '-';
    return ebase36(buf+1,-n)+1;
  }
  *buf = B36[0]; buf[1] = 0;
  if(n == 0) return 0;
  int i = ebase36(buf,n/36);
  buf[i] = B36[n%36]; buf[i+1] = 0;
  return i+1;
}

int dbase36(const char *buf) {
  if(*buf == '-') return -dbase36(buf+1);
  int n = 0;
  while(*buf != 0) {
    n *= 36;
    n += index(B36,*buf)-B36;
    buf++;
  }
  return n;
}

char* mkpath(char *p, int x, int z) {
  p = mkb64(p,((x%64)+64)%64,((z%64)+64)%64,"/");
  p = stpcpy(p,"/c."); p = mkb64(p,x,z,".");
  p = stpcpy(p,".dat"); return p;
}

char* mkb64(char *p, int x, int z, const char *d) {
  ebase36(p,x); p = index(p,0);
  p = stpcpy(p,d); ebase36(p,z);
  return index(p,0);
}

size_t region_read(region* r, char *d, size_t n) {
  int ret; char space[64];
  r.s.avail_out = n;
  r.s.next_out = d;
  while(r.s.avail_out > 0) {
    r.s.avail_in = fread(space, 1, 64, r.f);
    if(ferror(r.f)) {
      
    }
  }
}

size_t region_write(region* r, char *d, size_t n) {
  
}

void tag_destroy(tag *t) {
  if(t == NULL) return; int i;
  if(t->type == 9 || t->type == 10) {
    for(i = 0; i < t->length; i++) {
      tag_destroy((tag*)t->data+i);
    }
  }
  if(t->name != NULL) free(t->name);
  if(t->data != NULL) free(t->data);
}

void tag_read(tag *t, region *r) {
  unsigned char c;
  unsigned short s;
  unsigned int i;
  tag *ts;
  switch(t->type) {
  case 1: //Byte
    t->data = malloc(1);
    region_read(r,t->data,1);
    t->length = 0; break;
  case 2: //Short
    t->data = malloc(2);
    region_read(r,t->data,2);
    *(short*)t->data = ntohs(*(short*)t->data);
    t->length = 0; break;
  case 3: //Int
    t->data = malloc(4);
    region_read(r,t->data,4);
    *(int*)t->data = ntohl(*(int*)t->data);
    t->length = 0; break;
  case 4: //Long (Long)
    t->data = malloc(8);
    region_read(r,t->data,8);
    *(long long*)t->data = htonll(*(long long*)t->data);
    t->length = 0; break;
  case 5: //Float
    t->data = malloc(4);
    region_read(r,t->data,4);
    *(int*)t->data = ntohl(*(int*)t->data);
    t->length = 0; break;
  case 6: //Double
    t->data = malloc(8);
    region_read(r,t->data,8);
    *(long long*)t->data = htonll(*(long long*)t->data);
    t->length = 0; break;
  case 7: //Bytes
    region_read(r,&i,4);
    i = ntohl(i); t->length = i;
    t->data = i>0 ? malloc(i) : NULL;
    region_read(r,t->data,i);
    break;
  case 8: //String
    region_read(r,&s,2);
    s = ntohs(s); t->length = s;
    t->data = s>0 ? malloc(s+1) : NULL;
    if(!t->data) break;
    region_read(r,t->data,s);
    *((char*)(t->data)+s) = 0;
    break;
  case 9: //List
    region_read(r,&c,1);
    region_read(r,&i,4);
    i = ntohl(i);
    t->length = i;
    if(i == 0) {
      ts = malloc(sizeof(tag));
      ts->type = c; t->data = ts;
      ts->nlen= 0; ts->name = NULL;
      break;
    }
    ts = malloc(i*sizeof(tag));
    t->data = ts;
    while(i-- > 0) {
      ts->type = c;
      ts->nlen = 0;
      ts->name = NULL;
      ts->parent = t;
      tag_read(ts,r);
      ts++;
    }
    break;
  case 10: //Compound
    t->length = 0; ts = NULL;
    while(t->length == 0 || (ts+t->length-1)->type != 0) {
      ts = realloc(ts,sizeof(tag)*(t->length+1));
      (ts+t->length)->parent = t; tag_parse(ts+t->length++,r);
    }
    if(t->length > 0) {
      t->length--;
      ts = realloc(ts,sizeof(tag)*t->length);
    }
    t->data = ts;
  }
}

void tag_write(tag *t, region *r) {
  unsigned char c;
  unsigned short s;
  unsigned int i;
  unsigned long long l;
  tag *ts; tag q;
  switch(t->type) {
  case 1: //Byte
    region_write(r,t->data,1); break;
  case 2: //Short
    s = htons(*(short*)t->data);
    region_write(r,&s,2); break;
  case 3: //Int
    i = htonl(*(int*)t->data);
    region_write(r,&i,4); break;
  case 4: //Long (Long)
    l = htonll(*(long long*)t->data);
    region_write(r,&l,8); break;
  case 5: //Float
    i = htonl(*(int*)t->data);
    region_write(r,&i,4); break;
  case 6: //Double
    l = htonll(*(long long*)t->data);
    region_write(r,&l,8); break;
  case 7: //Bytes
    i = htonl(t->length);
    region_write(r,&i,4);
    region_write(r,t->data,t->length);
    break;
  case 8: //String
    s = htons(t->length);
    region_write(r,&s,2);
    region_write(r,t->data,t->length);
    break;
  case 9: //List
    ts = (tag*)t->data;
    c = ts->type;
    region_write(r,&c,1);
    i = t->length;
    i = htonl(t->length);
    region_write(r,&i,4);
    ts = t->data;
    i = t->length;
    while(i-- > 0) {
      tag_write(ts++,r);
    }
    break;
  case 10: //Compound
    ts = t->data;
    i = t->length;
    while(i-- > 0) {
      tag_serial(ts++,r);
    }
    q.type = 0;
    tag_serial(&q,r);
  }
}

void tag_parse(tag *t, region *r) {
  unsigned char c;
  region_read(r,&c,1);
  t->type = c;
  if(c == 0) return;
  region_read(r,&(t->nlen),2);
  t->nlen = ntohs(t->nlen);
  t->name = malloc(t->nlen+1);
  region_read(r,t->name,t->nlen);
  t->name[t->nlen] = 0;
  tag_read(t,r);
}

void tag_serial(tag *t, region *r) {
  unsigned char c = t->type;
  unsigned short s;
  region_write(r,&c,1);
  if(c == 0) return;
  s = htons(t->nlen);
  region_write(r,&s,2);
  region_write(r,t->name,t->nlen);
  tag_write(t,r);
}

char* tag_str(tag *t, char *s, size_t n) {
  signed long long m = n;
  m -= 1; int c; int i;
  switch(t->type) {
  case 1: //Byte
    c = snprintf(s,m,"'\\x%02x'",*(char*)t->data); s += c; m -= c; break;
  case 2: //Short
    c = snprintf(s,m,"%u",*(short*)t->data); s += c; m -= c; break;
  case 3: //Int
    c = snprintf(s,m,"%i",*(int*)t->data); s += c; m -= c; break;
  case 4: //Long (Long)
    c = snprintf(s,m,"%lld",*(long long*)t->data); s += c; m -= c; break;
  case 5: //Float
    c = snprintf(s,m,"%f",*(float*)t->data); s += c; m -= c; break;
  case 6: //Double
    c = snprintf(s,m,"%f",*(double*)t->data); s += c; m -= c; break;
  case 7: //Bytes
    for(i = 0; i < t->length && m > 0; i++) {
      c = snprintf(s,m,"%02x",((char*)t->data)[i]);
      s += c; m -= c;
    }
    break;
  case 8: //String
    *(s++) = '"';
    if(t->length < m) {
      strncpy(s,(char*)t->data,t->length);
      s += t->length;
    } else {
      strncpy(s,(char*)t->data,m); s += m;
    }
    *(s++) = '"'; break;
  case 9: //List
  case 10: //Compound
    break;
  }
  if(m <= 3) s[-3] = s[-2] = s[-1] = '.';
  *s = 0; return s;
}

int tag_find(tag *t, tag **res, int count, short nlen, char *name) {
  if(t == NULL || count < 1) return 0; int c = 0; int h; int i; tag *u;
  if(t->type == 9 || t->type == 10) {
    tag* tags = t->data;
    for(i = 0; i < t->length; i++) {
      if(count < 1) return c;
      if(tags[i].nlen == nlen && strncmp(tags[i].name,name,nlen) == 0) {
	*(res++) = tags+i; count--; c++;
      }
    }
    if(count < 1) return c;
    for(i = 0; i < t->length; i++) {
      if(count < 1) return c;
      h = tag_find(tags+i,res,count,nlen,name);
      count -= h; c += h; res += h;
    }
  }
  return c;
}

int tag_finds(tag *t, tag **res, int count, char *name) {
  return tag_find(t,res,count,strlen(name),name);
}

void tag_tree(tag *t) {
  tag_tree_(t,0);
}

void tag_tree_(tag *t, int n) {
  if(t == NULL) return; int i;
  char buffer[65536]; char *s = buffer;
  memset(s,' ',n); s += n;
  s += sprintf(s,"TAG(%i,\"",t->type);
  strncpy(s,t->name,t->nlen); s += t->nlen;
  s += sprintf(s,"\",");
  s = tag_str(t,s,65536-(s-buffer));
  printf("%s) @%p,%p\n",buffer,t,t->data);
  if(t->type == 9 || t->type == 10) {
    for(i = 0; i < t->length; i++) {
      tag_tree_((tag*)t->data+i,n+2);
    }
  }
}

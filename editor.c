#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <dirent.h>
#include <zlib.h>

#define WCACHESIZE 16

typedef struct tag tag;

struct tag {
  char *name;
  unsigned short nlen;
  tag *parent;
  unsigned char type;
  size_t length;
  void *data;
};

typedef struct chunk chunk;

struct chunk {
  int x, z;
  FILE *f;
  gzFile *g;
  unsigned int* table;
  void *data;
  void *mark;
  size_t len;
  char mode;
  char ctype;
  char buf[64];
  z_stream s;
};

typedef struct world world;

struct world {
  char *path;
  char *pmark;
  char wtype;
  chunk *cache[WCACHESIZE];
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
world* world_open(const char *path);
chunk* world_get(world *w, int x, int z);
int world_foreach(world *w, void *ctx, int(*f)(chunk *r, int x, int z, void *ctx));
int world_close(world *w);
chunk* chunk_openr(const char *filename, const char *mode);
chunk* chunk_opengz(const char *filename, const char *mode);
chunk* chunk_mem(char *data, size_t len, const char *mode);
int chunk_begin(chunk *r, int x, int z);
int chunk_finish(chunk *r);
int chunk_close(chunk *r);
size_t chunk_read(chunk* r, void *d, size_t n);
size_t chunk_write(chunk* r, void *d, size_t n);
void tag_destroy(tag *t);
void tag_read(tag *t, chunk *r);
void tag_write(tag *t, chunk *r);
void tag_parse(tag *t, chunk *r);
void tag_serial(tag *t, chunk *r);
char* tag_str(tag *t, char *s, size_t n);
int tag_find(tag *t, tag **res, int count, char *name);
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

char* mkgpath(char *p, int x, int z) {
  p = mkb64(p,((x%64)+64)%64,((z%64)+64)%64,"/");
  p = stpcpy(p,"/c."); p = mkb64(p,x,z,".");
  p = stpcpy(p,".dat"); return p;
}

char* mkzpath(char *p, int x, int z) {
  x = x / 32; z = z / 32;
  p = stpcpy(p,"region/r."); p = mkb64(p,x,z,".");
  p = stpcpy(p,".mcr"); return p;
}

char* mkb64(char *p, int x, int z, const char *d) {
  ebase36(p,x); p = index(p,0);
  p = stpcpy(p,d); ebase36(p,z);
  return index(p,0);
}

world* world_open(const char *path) {
  char buffer[65536];
  int plen = strlen(path);
  if(plen > 65524) return NULL;
  memcpy(buffer,path,plen);
  buffer[plen++] = '/';
  strcpy(buffer+plen,"/level.dat");
  chunk *flevel = chunk_opengz(buffer,"r");
  if(!flevel) return NULL;
  tag level; level.data = NULL;
  tag_parse(&level,flevel);
  chunk_close(flevel);
  if(!level.data) return NULL;
  world *w = malloc(sizeof(world));
  w->path = malloc(65536);
  memcpy(w->path,buffer,plen+1);
  w->pmark = w->path + plen;
  bzero(w->cache,sizeof(chunk*)*WCACHESIZE);
  tag *version; w->wtype = 0;
  if(tag_find(&level,&version,1,"version")) {
    if(*((int*)version->data) == 19132) {
      w->wtype = 1;
    }
  }
  tag_destroy(&level);
  return w;
}

chunk* world_get(world *w, int x, int z) {
  chunk *r = NULL;
  /*
  for(int i = 0; i < WCACHESIZE; i++) {
    if(w->cache[i] && w->cache[i]->x == x && w->cache[i]->z == z) {
      while(++i < WCACHESIZE) {
	cache *t = w->cache[i];
	w->cache[i] = w->cache[i-1];
	w->cache[i-1] = t;
      }
      if(w->cache[i]->ctype & 0xf == 0x2) {
	chunk_begin(r,x,z);
      }
      return w->cache[WCACHESIZE-1];
    }
  }
  */
  switch(w->wtype) {
  case 0x00:
    mkgpath(w->pmark,x,z);
    r = chunk_opengz(w->path,"r");
    break;
  case 0x01:
    mkzpath(w->pmark,x,z);
    r = chunk_openr(w->path,"r+");
    if(!r) break;
    if(!chunk_begin(r,x,z)) {
      chunk_close(r);
      r = NULL;
    }
  }
  /*
  if(!r) return r;
  for(int i = 1; i < WCACHESIZE; i++) {
    cache *t = w->cache[i];
    w->cache[i] = w->cache[i-1];
    w->cache[i-1] = t;
  }
  w->cache[WCACHESIZE-1] = r;
  */
  return r;
}

int world_foreach(world *w, void *ctx, int(*f)(chunk *r, int x, int z, void *ctx)) {
  DIR *d; struct dirent *dp;
  int x; int z; int sx; int sz;
  char *ptr; char *pptr;
  char *end;
  switch(w->wtype) {
  case 0x00:
    for(x = 0; x < 64; x++) {
      for(z = 0; z < 64; z++) {
	end = mkb64(w->pmark,x,z,"/");
	*(end++) = '/'; *end = 0;
	if((d = opendir(w->path)) == NULL) continue;
	while((dp = readdir(d)) != NULL) {
	  if(dp->d_name[0] == 'c') {
            strcpy(end,dp->d_name);
	    ptr = dp->d_name+2; pptr = ptr;
	    strsep(&pptr,"."); sx = dbase36(ptr);
	    ptr = pptr; strsep(&pptr,"."); sz = dbase36(ptr);
	    chunk* c = chunk_opengz(w->path,"r");
	    if(c) {
	      int r = f(c,sx,sz,ctx);
	      chunk_close(c);
	      if(r) {
		closedir(d);
		return r;
	      }
	    }
	  }
	}
	closedir(d);
      }
    }
    return 0;
  case 0x01:
    end = stpcpy(w->pmark,"/region/");
    if((d = opendir(w->path)) == NULL) break;
    while((dp = readdir(d)) != NULL) {
      if(dp->d_name[0] == 'r') {
        strcpy(end,dp->d_name);
	ptr = dp->d_name+2; pptr = ptr;
	strsep(&pptr,"."); sx = atoi(ptr);
	ptr = pptr; strsep(&pptr,"."); sz = atoi(ptr);
	chunk* c = chunk_openr(w->path,"r+");
	if(!c) continue;
	for(z = 0; z < 32; z++) {
	  for(x = 0; x < 32; x++) {
	    if(chunk_begin(c,x,z)) {
	      int r = f(c,32*sx+x,32*sz+z,ctx);
	      if(r) {
		chunk_finish(c);
		chunk_close(c);
		closedir(d);
		return r;
	      }
	    }
	    chunk_finish(c);
	  }
	}
	chunk_close(c);
      }
    }
    closedir(d);
    return 0;
  }
  return 0;
}

int world_close(world *w) {
  free(w->path);
  free(w);
}

chunk* chunk_openr(const char *filename, const char *mode) {
  if(*mode != 'r' && *mode != 'w') return NULL;
  if(mode[1] != '+' || mode[2] != 0) return NULL;
  chunk *r = malloc(sizeof(chunk));
  bzero(r,sizeof(chunk));
  r->f = fopen(filename,mode);
  if(!r->f) return NULL;
  r->table = malloc(1024*sizeof(int));
  bzero(r->table,1024*sizeof(int));
  fread(r->table,sizeof(int),1024,r->f);

  r->mode = *mode;
  r->ctype = 2;
  /*
  r->s.zalloc = Z_NULL;
  r->s.zfree = Z_NULL;
  r->s.opaque = Z_NULL;
  if(r->mode == 'r') {
    int ret = inflateInit(&(r->s));
    if(ret != Z_OK) return NULL;
  } else {
    int ret = deflateInit(&(r->s),Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK) return NULL;
  }
  r->s.next_in = r->buf;
  r->s.avail_in = 0;
  */
  return r;
}

chunk* chunk_opengz(const char *filename, const char *mode) {
  if(*mode != 'r' && *mode != 'w') return NULL;
  if(mode[1] != 0) return NULL;
  chunk *r = malloc(sizeof(chunk));
  bzero(r,sizeof(chunk));
  r->g = gzopen(filename,mode);
  if(!r->g) {
    free(r);
    return NULL;
  }
  r->mode = *mode;
  r->ctype = 0x21;
  return r;
}

chunk* chunk_mem(char *data, size_t len, const char *mode) {
  if(*mode != 'r' && *mode != 'w') return NULL;
  if(mode[1] != '+' || mode[2] != 0) return NULL;
  chunk *r = malloc(sizeof(chunk));
  r->mode = *mode;
  r->ctype = 0x12;
  r->s.zalloc = Z_NULL;
  r->s.zfree = Z_NULL;
  r->s.opaque = Z_NULL;
  if(r->mode == 'r') {
    int ret = inflateInit(&(r->s));
    if(ret != Z_OK) return NULL;
  } else {
    int ret = deflateInit(&(r->s),Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK) return NULL;
  }
  r->s.next_in = data;
  r->s.avail_in = len;
  r->data = data;
  r->len = len;
  return r;
}

int chunk_begin(chunk *r, int x, int z) {
  x = (x%32+32)%32; z = (z%32+32)%32;
  unsigned int pos = r->table[32*z+x];
  //if(fseek(r->f,4*(x+z*32),SEEK_SET)) return 0;
  //if(fread(&pos,4,1,r->f) != 1) return 0;
  if(!pos) return 0;
  if(ntohl(pos) != pos) {
    pos = pos & 0x00ffffff;
    pos = ntohl(pos);
  }
  pos >>= 8;
  pos <<= 12;
  if(fseek(r->f,pos+4,SEEK_SET)) return 0;
  if(ftell(r->f) != pos+4) return 0;
  if(fread(&(r->ctype),1,1,r->f) != 1) return 0;
  r->s.zalloc = Z_NULL;
  r->s.zfree = Z_NULL;
  r->s.opaque = Z_NULL;
  if(r->mode == 'r') {
    int ret = inflateInit(&(r->s));
    if(ret != Z_OK) return 0;
  } else {
    int ret = deflateInit(&(r->s),Z_DEFAULT_COMPRESSION);
    if(ret != Z_OK) return 0;
  }
  r->s.next_in = NULL;
  r->s.avail_in = 0;
  return 1;
}

int chunk_finish(chunk *r) {
  if(r == NULL) return 0;
  if(r->mode == 'r') {
    (void)inflateEnd(&(r->s));
  } else {
    r->s.avail_in = 0;
    r->s.next_in = NULL;
    (void)deflate(&(r->s),Z_FINISH);
    (void)deflateEnd(&(r->s));
  }
  return 1;
}

int chunk_close(chunk *r) {
  if(r == NULL) return 0;
  switch(r->ctype) {
  case 0x21:
    return gzclose(r->g);
  case 0x02:
    /*
    if(r->mode == 'r') {
      (void)inflateEnd(&(r->s));
    } else {
      (void)deflateEnd(&(r->s));
    }
    */
    free(r->table);
    return fclose(r->f);
  }
}

size_t chunk_read(chunk* r, void *d, size_t n) {
  if(r->mode != 'r') return 0;
  int ret;
  switch(r->ctype) {
  case 0x12:
  case 0x02:
    r->s.avail_out = n;
    r->s.next_out = d;
    while(r->s.avail_out > 0) {
      switch(inflate(&(r->s),Z_NO_FLUSH)) {
      case Z_STREAM_ERROR:
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
	(void)inflateEnd(&(r->s));
	return -1;
      case Z_STREAM_END:
	break;
      }
      if(r->s.avail_out == 0) break;
      if(r->ctype == 0x02) {
	r->s.avail_in = fread(r->buf,1,64,r->f);
	if(ferror(r->f)) {
	  (void)inflateEnd(&(r->s));
	  return -1;
	}
	r->s.next_in = r->buf;
      }
      if(r->s.avail_in == 0) break;
    }
    return n-r->s.avail_out;
  case 0x21:
    return gzread(r->g,d,n);
  }
  return 0;
}

size_t chunk_write(chunk* r, void *d, size_t n) {
  if(r->mode != 'w') return 0;
  if(r->mode != 'r') return 0;
  int ret, rem;
  switch(r->ctype) {
  case 0x12:
  case 0x02:
    if(r->ctype == 0x02) {
      r->s.avail_in = n;
      r->s.next_in = d;
    }
    while(r->s.avail_in > 0) {
      r->s.avail_out = 64;
      r->s.next_out = r->buf;
      switch(deflate(&(r->s),Z_NO_FLUSH)) {
      case Z_STREAM_ERROR:
	(void)deflateEnd(&(r->s));
	return -1;
      case Z_STREAM_END:
	break;
      }
      if(r->ctype == 0x02) {
	rem = 64 - r->s.avail_out;
	if(fwrite(r->buf,1,rem,r->f) != rem || ferror(r->f)) {
	  (void)deflateEnd(&(r->s));
	  return -1;
	}
      }
      if(r->s.avail_out == 0) break;
    }
    return n-r->s.avail_in;
  case 0x21:
    return gzwrite(r->g,d,n);
  }
  return 0;
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

void tag_read(tag *t, chunk *r) {
  unsigned char c;
  unsigned short s;
  unsigned int i;
  tag *ts;
  switch(t->type) {
  case 1: //Byte
    t->data = malloc(1);
    chunk_read(r,t->data,1);
    t->length = 0; break;
  case 2: //Short
    t->data = malloc(2);
    chunk_read(r,t->data,2);
    *(short*)t->data = ntohs(*(short*)t->data);
    t->length = 0; break;
  case 3: //Int
    t->data = malloc(4);
    chunk_read(r,t->data,4);
    *(int*)t->data = ntohl(*(int*)t->data);
    t->length = 0; break;
  case 4: //Long (Long)
    t->data = malloc(8);
    chunk_read(r,t->data,8);
    *(long long*)t->data = htonll(*(long long*)t->data);
    t->length = 0; break;
  case 5: //Float
    t->data = malloc(4);
    chunk_read(r,t->data,4);
    *(int*)t->data = ntohl(*(int*)t->data);
    t->length = 0; break;
  case 6: //Double
    t->data = malloc(8);
    chunk_read(r,t->data,8);
    *(long long*)t->data = htonll(*(long long*)t->data);
    t->length = 0; break;
  case 7: //Bytes
    chunk_read(r,&i,4);
    i = ntohl(i); t->length = i;
    t->data = i>0 ? malloc(i) : NULL;
    chunk_read(r,t->data,i);
    break;
  case 8: //String
    chunk_read(r,&s,2);
    s = ntohs(s); t->length = s;
    t->data = s>0 ? malloc(s+1) : NULL;
    if(!t->data) break;
    chunk_read(r,t->data,s);
    *((char*)(t->data)+s) = 0;
    break;
  case 9: //List
    chunk_read(r,&c,1);
    chunk_read(r,&i,4);
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

void tag_write(tag *t, chunk *r) {
  unsigned char c;
  unsigned short s;
  unsigned int i;
  unsigned long long l;
  tag *ts; tag q;
  switch(t->type) {
  case 1: //Byte
    chunk_write(r,t->data,1); break;
  case 2: //Short
    s = htons(*(short*)t->data);
    chunk_write(r,&s,2); break;
  case 3: //Int
    i = htonl(*(int*)t->data);
    chunk_write(r,&i,4); break;
  case 4: //Long (Long)
    l = htonll(*(long long*)t->data);
    chunk_write(r,&l,8); break;
  case 5: //Float
    i = htonl(*(int*)t->data);
    chunk_write(r,&i,4); break;
  case 6: //Double
    l = htonll(*(long long*)t->data);
    chunk_write(r,&l,8); break;
  case 7: //Bytes
    i = htonl(t->length);
    chunk_write(r,&i,4);
    chunk_write(r,t->data,t->length);
    break;
  case 8: //String
    s = htons(t->length);
    chunk_write(r,&s,2);
    chunk_write(r,t->data,t->length);
    break;
  case 9: //List
    ts = (tag*)t->data;
    c = ts->type;
    chunk_write(r,&c,1);
    i = t->length;
    i = htonl(t->length);
    chunk_write(r,&i,4);
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

void tag_parse(tag *t, chunk *r) {
  unsigned char c;
  chunk_read(r,&c,1);
  t->type = c;
  if(c == 0) return;
  chunk_read(r,&(t->nlen),2);
  t->nlen = ntohs(t->nlen);
  t->name = malloc(t->nlen+1);
  chunk_read(r,t->name,t->nlen);
  t->name[t->nlen] = 0;
  tag_read(t,r);
}

void tag_serial(tag *t, chunk *r) {
  unsigned char c = t->type;
  unsigned short s;
  chunk_write(r,&c,1);
  if(c == 0) return;
  s = htons(t->nlen);
  chunk_write(r,&s,2);
  chunk_write(r,t->name,t->nlen);
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

int tag_find(tag *t, tag **res, int count, char *name) {
  if(t == NULL || count < 1) return 0; int c = 0; int h; int i; tag *u;
  if(t->type == 9 || t->type == 10) {
    tag* tags = t->data;
    for(i = 0; i < t->length; i++) {
      if(count < 1) return c;
      if(tags[i].name && strcmp(tags[i].name,name) == 0) {
	*(res++) = tags+i; count--; c++;
      }
    }
    if(count < 1) return c;
    for(i = 0; i < t->length; i++) {
      if(count < 1) return c;
      h = tag_find(tags+i,res,count,name);
      count -= h; c += h; res += h;
    }
  }
  return c;
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

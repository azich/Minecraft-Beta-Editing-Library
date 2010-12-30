#include "editor.c"

void nprint(int nlen, char *name) {
  int i;
  for(i = 0; i < nlen; i++) {
    printf("%c",name[i]);
  }
  printf("\n");
}

int main() {
  tag t; tag *tt; tag *tu; int i;
  do {
    gzFile *f = gzopen("world/0/0/c.0.0.dat","r");
    tt = NULL;
    tag_parse(&t,f); gzclose(f);
    tag_find(&t,&tt,1,8,"Entities");
    if(tt == NULL) {
      printf("Entities not found\n");
      break; //continue;
    }
    if(tt->length < 1) {
      printf("No entities\n");
      break; //continue;
    }
    for(i = 0; i < tt->length; i++) {
      int type = ((tag*)tt->data+i)->type;
      if(type == 63 || type == 68) {
	tu = NULL; tag_find((tag*)tt->data+i,&tu,1,5,"Text1");
	if(tu == NULL) continue; nprint(tu->nlen,tu->name);
	printf("\n");
      }
    }
  } while(0);
}

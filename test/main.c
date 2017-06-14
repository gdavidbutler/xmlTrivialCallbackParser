#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "xml.h"

void
cb(
  xmlTp_t typ
 ,unsigned int l
 ,const xmlSt_t *tg
 ,const xmlSt_t *nm
 ,const xmlSt_t *vl
 ,void *v
){
  (void)v;
  switch (typ) {
  case xmlTp_Eb:
    printf("B /%.*s", tg->l, tg->s);
    for (l--, tg++; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    printf("\n");
    break;
  case xmlTp_Ea:
    printf("A /%.*s", tg->l, tg->s);
    for (l--, tg++; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    {
      char *d;

      if (!(d = malloc(vl->l + 1))
       || xmlDecode(d, vl->s, vl->l) != (int)vl->l)
        printf(":\"%.*s\"=\"%.*s\"\n", nm->l, nm->s, vl->l, vl->s);
      else
        printf(":\"%.*s\"=\"%.*s\"(%s)\n", nm->l, nm->s, vl->l, vl->s, d);
      free(d);
    }
    break;
  case xmlTp_Ee:
    printf("E /%.*s", tg->l, tg->s);
    for (l--, tg++; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    {
      char *d;

      if (!(d = malloc(vl->l + 1))
       || xmlDecode(d, vl->s, vl->l) != (int)vl->l)
        printf("(%.*s)\n", vl->l, vl->s);
      else
        printf("(%.*s)(%s)\n", vl->l, vl->s, d);
      free(d);
    }
    break;
  case xmlTp_Er:
    printf("! %.*s", tg->l, tg->s);
    for (l--, tg++; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    printf(":%.*s=\"%.*s\"\n", nm->l, nm->s, vl->l, vl->s);
    break;
  }
  return;
}

int
main(
  int argc
 ,char *argv[]
){
  int fd;
  int sz;
  char *bf;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s [0|1] file\n", argv[0]);
    return 1;
  }
  if ((fd = open(argv[2], O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[2]);
    return 1;
  }
  sz = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  bf = malloc(sz + 1);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[2]);
    return 1;
  }
  close(fd);
  bf[sz] = '\0';

  printf("%d %d\n", sz, xmlParse(atoi(argv[1]) ? cb : 0, bf, 0));

  free(bf);
  return 0;
}

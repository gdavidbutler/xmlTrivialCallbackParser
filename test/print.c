#include <stdio.h>
#include <stdlib.h>
#include "xml.h"

static int P;

static int
cb(
  xmlTp_t typ
 ,unsigned int l
 ,const xmlSt_t *tg
 ,const xmlSt_t *nm
 ,const xmlSt_t *vl
 ,void *v
){
  int i;

  (void)v;
  switch (typ) {
  case xmlTp_Eb:
    if (P) {
      putchar('>');
      putchar('\n');
    }
    P = 1;
    for (i = 1; i < (int)l; ++i)
      putchar(' ');
    printf("<%.*s", (tg + l - 1)->l, (tg + l - 1)->s);
    break;
  case xmlTp_Ea:
    printf(" \"%.*s\"", nm->l, nm->s);
    if (vl->l) {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("=\"%.*s\"", vl->l, vl->s);
      else
        printf("=\"%.*s\"", i, d);
      free(d);
    }
    break;
  case xmlTp_Ee:
    if (!vl->l) {
      if (*(tg + l - 1)->s == '?') {
        putchar('?');
      } else if (*(tg + l - 1)->s != '!') {
        putchar('>');
        putchar('\n');
        for (i = 1; i < (int)l; ++i)
          putchar(' ');
        printf("</%.*s", (tg + l - 1)->l, (tg + l - 1)->s);
      }
    } else {
      unsigned char *d;

      putchar('>');
      putchar('\n');
      for (i = 0; i < (int)l; ++i)
        putchar(' ');
      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("%.*s\n", vl->l, vl->s);
      else
        printf("%.*s\n", i, d);
      free(d);
      for (i = 1; i < (int)l; ++i)
        putchar(' ');
      printf("</%.*s", (tg + l - 1)->l, (tg + l - 1)->s);
    }
    break;
  case xmlTp_Er:
    printf("! ");
    if (l) {
      printf("%.*s", tg->l, tg->s);
      for (l--, tg++; l; l--, tg++)
        printf("/%.*s", tg->l, tg->s);
    }
    printf(":%.*s=(%.*s)", nm->l, nm->s, vl->l, vl->s);
    break;
  }
  return 0;
}

int
main(
){
  unsigned char *bf;
  int sz;
  int i;

  if (!(bf = malloc(BUFSIZ)))
    return -1;
  sz = 0;
  while ((i = fread(bf + sz, 1, BUFSIZ, stdin)) > 0) {
    sz += i;
    if (!(bf = realloc(bf, sz + BUFSIZ)))
      return -1;
  }
  *(bf + sz) = '\0';
  P = 0;
  xmlParse(cb, bf, 0);
  if (P) {
    putchar('>');
    putchar('\n');
  }
  return 0;
}

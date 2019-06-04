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
    } else
      P = 1;
    for (i = 1; i < (int)l; ++i)
      putchar(' ');
    printf("<%.*s", (tg + l - 1)->l, (tg + l - 1)->s);
    break;
  case xmlTp_Ea:
    if (nm)
      printf(" %.*s=", nm->l, nm->s);
    else
      printf(" ");
    if (vl->l) {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("\"%.*s\"", vl->l, vl->s);
      else
        printf("\"%.*s\"", i, d);
      free(d);
    }
    break;
  case xmlTp_Ec:
    {
      unsigned char *d;

      if (P) {
        putchar('>');
        putchar('\n');
        P = 0;
      }
      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("%.*s\n", vl->l, vl->s);
      else
        printf("%.*s\n", i, d);
      free(d);
    }
    break;
  case xmlTp_Ee:
    if (*(tg + l - 1)->s == '?')
      putchar('?');
    else if (*(tg + l - 1)->s != '!') {
      if (P) {
        putchar('>');
        putchar('\n');
      }
      for (i = 1; i < (int)l; ++i)
        putchar(' ');
      printf("</%.*s", (tg + l - 1)->l, (tg + l - 1)->s);
    }
    putchar('>');
    putchar('\n');
    P = 0;
    break;
  }
  return 0;
}

int
main(
){
  unsigned char *bf;
  xmlSt_t tg[64];
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
  P = 0;
  xmlParse(cb, sizeof(tg) / sizeof(tg[0]), tg, 0, bf, sz, 0);
  if (P) {
    putchar('>');
    putchar('\n');
  }
  return 0;
}

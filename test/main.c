/*
 * xmlTrivialCallbackParser - a XML callback parser in C
 * Copyright (C) 2018 G. David Butler <gdb@dbSystems.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "xml.h"

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
    printf("B ");
    for (; l; l--, tg++)
      if (tg->o)
        printf("/%.*s|%.*s", tg->o - 1, tg->s, tg->l - tg->o, tg->s + tg->o);
      else
        printf("/%.*s", tg->l, tg->s);
    printf(" @%p\n", (void *)vl->s);
    break;
  case xmlTp_Ea:
    printf("A ");
    for (; l; l--, tg++)
      if (tg->o)
        printf("/%.*s|%.*s", tg->o - 1, tg->s, tg->l - tg->o, tg->s + tg->o);
      else
        printf("/%.*s", tg->l, tg->s);
    if (nm) {
      if (nm->o)
        printf("~%.*s|%.*s=", nm->o - 1, nm->s, nm->l - nm->o, nm->s + nm->o);
      else
        printf("~%.*s=", nm->l, nm->s);
    } else
      printf("~");
    {
      unsigned char *d;

      if (vl->o)
        printf("%.*s|", vl->o - 1, vl->s);
      i = -1;
      if (!(d = malloc(vl->l - vl->o))
       || (i = xmlDecodeBody(d, vl->l - vl->o, vl->s + vl->o, vl->l - vl->o)) < 0
       || i > (int)(vl->l - vl->o))
        printf("%.*s(%d|%u)\n", vl->l - vl->o, vl->s + vl->o, i, vl->l - vl->o);
      else
        printf("%.*s(%.*s)\n", vl->l - vl->o, vl->s + vl->o, i, d);
      free(d);
    }
    break;
  case xmlTp_Ec:
    printf("C ");
    for (; l; l--, tg++)
      if (tg->o)
        printf("/%.*s|%.*s", tg->o - 1, tg->s, tg->l - tg->o, tg->s + tg->o);
      else
        printf("/%.*s", tg->l, tg->s);
    printf("~");
    {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("(%.*s)(%d|%u)\n", vl->l, vl->s, i, vl->l);
      else
        printf("(%.*s)(%.*s)\n", vl->l, vl->s, i, d);
      free(d);
    }
    break;
  case xmlTp_Ee:
    printf("E ");
    for (; l; l--, tg++)
      if (tg->o)
        printf("/%.*s|%.*s", tg->o - 1, tg->s, tg->l - tg->o, tg->s + tg->o);
      else
        printf("/%.*s", tg->l, tg->s);
    printf(" @%p\n", (void *)vl->s);
    break;
  }
  return (0);
}

int
main(
  int argc
 ,char *argv[]
){
  xmlSt_t tg[64];
  int fd;
  int sz;
  unsigned char *bf;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s 0|1 file\n", argv[0]);
    return (1);
  }
  if ((fd = open(argv[2], O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[2]);
    return (1);
  }
  sz = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  bf = malloc(sz);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[2]);
    return (1);
  }
  close(fd);
  printf("%d %d\n", sz, xmlParse(atoi(argv[1]) ? cb : 0, sizeof (tg) / sizeof (tg[0]), tg, bf, sz, 0));
  free(bf);
  return (0);
}

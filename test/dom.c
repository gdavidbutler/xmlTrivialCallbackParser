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
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "xml.h"

static void
xmlNodeVisit(
  xmlNode_t *n
 ,unsigned int d
 ,xmlNodeVisit_t v
 ,void *c
){
  unsigned int i;

  (void)c;
  if (!d) /* skip "document" node */
    return;
  switch (v) {

  case xmlNodeVisitPreorder:
    printf("E %u %u %.*s", d, n->parent->nodeW, n->value.l, n->value.s);
    for (i = 0; i < n->attributeN; ++i)
      printf(" (%.*s=%.*s)"
       ,(n->attribute + i)->name.l, (n->attribute + i)->name.s
       ,(n->attribute + i)->value.l, (n->attribute + i)->value.s
      );
    putchar('\n');
    break;

  case xmlNodeVisitInorder:
    break;

  case xmlNodeVisitPostorder:
    break;

  case xmlNodeVisitLeaf:
    if (!n->beg) {
      unsigned char *s;
      int j;

      j = -1;
      if (!(s = malloc(n->value.l))
       || (j = xmlDecodeBody(s, n->value.l, n->value.s, n->value.l)) < 0
       || j > (int)n->value.l)
        printf("C %u %u (%.*s)(%d|%u)\n", d, n->parent->nodeW, n->value.l, n->value.s, j, n->value.l);
      else
        printf("C %u %u %.*s\n", d, n->parent->nodeW, j, s);
      free(s);
    } else {
      printf("E %u %u %.*s", d, n->parent->nodeW, n->value.l, n->value.s);
      for (i = 0; i < n->attributeN; ++i)
        printf(" (%.*s=%.*s)"
         ,(n->attribute + i)->name.l, (n->attribute + i)->name.s
         ,(n->attribute + i)->value.l, (n->attribute + i)->value.s
        );
      putchar('\n');
    }
    break;
  }
}

int
main(
  int argc
 ,char *argv[]
){
  xmlNode_t doc; /* "document" node */
  xmlSt_t tg[64];
  int fd;
  int sz;
  unsigned char *bf;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s file\n", argv[0]);
    return (1);
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[1]);
    return (1);
  }
  sz = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  bf = malloc(sz);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[1]);
    return (1);
  }
  close(fd);
  memset(&doc, 0, sizeof(doc));
  printf("%d %d\n", sz, xml2node(realloc, &doc, tg, bf, sizeof (tg) / sizeof (tg[0]), sz, 0));
  xmlNodeWalk(&doc, xmlNodeVisit, 0);
  xmlNodeFree(free, &doc);
  free(bf);
  return (0);
}

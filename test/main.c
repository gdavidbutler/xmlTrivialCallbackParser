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
  unsigned int i;

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

      i = -1;
      if (!(d = malloc(vl->l + 1))
       || (i = xmlDecodeBody(d, vl->s, vl->l)) != vl->l)
        printf(":\"%.*s\"=\"%.*s\"(%u!=%u)\n", nm->l, nm->s, vl->l, vl->s, i, vl->l);
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

      i = -1;
      if (!(d = malloc(vl->l + 1))
       || (i = xmlDecodeBody(d, vl->s, vl->l)) != vl->l)
        printf("(%.*s)(%u!=%u)\n", vl->l, vl->s, i, vl->l);
      else
        printf("(%.*s)(%s)\n", vl->l, vl->s, d);
      free(d);
    }
    break;
  case xmlTp_Er:
    printf("! ");
    if (l) {
      printf("%.*s", tg->l, tg->s);
      for (l--, tg++; l; l--, tg++)
        printf("/%.*s", tg->l, tg->s);
    }
    printf(":%.*s=(%.*s)\n", nm->l, nm->s, vl->l, vl->s);
    break;
  }
  fflush(stdout);
  return;
}

int
main(
  int argc
 ,char *argv[]
){
  static const char enc[] = "this is a test <of>, <![CDATA[<hello>]]> & ]]>. how did it do?";
  static const char b64[] = " Q m F z Z T Y 0 ";
  static const unsigned char str[] = "Base64";
  int fd;
  int sz;
  char *bf;

  if (argc == 2) {
    if (!(bf = xmlEncodeString(enc, sizeof(enc) - 1)))
      return 2;
    printf("xmlEncodeString(%s)\n->\n%s\n", enc, bf);
    free(bf);
    putchar('\n');
    if (!(bf = xmlEncodeCdata(enc, sizeof(enc) - 1)))
      return 2;
    printf("xmlEncodeCdata(%s)\n->\n%s\n", enc, bf);
    free(bf);
    putchar('\n');
    sz = xmlDecodeBase64Need(sizeof(b64) - 1);
    if (!(bf = malloc(sz)))
      return 2;
    sz = xmlDecodeBase64((unsigned char *)bf, sz, b64, sizeof(b64) - 1);
    printf("xmlDecodeBase64(%s)\n->\n%.*s\n", b64, sz, bf);
    free(bf);
    putchar('\n');
    sz = xmlEncodeBase64Need(sizeof(str) - 1);
    if (!(bf = malloc(sz)))
      return 2;
    sz = xmlEncodeBase64(bf, sz, str, sizeof(str) - 1);
    printf("xmlEncodeBase64(%s)\n->\n%.*s\n", str, sz, bf);
    free(bf);
    return 0;
  }
  if (argc != 3) {
    fprintf(stderr, "Usage: %s any | 0|1 file\n", argv[0]);
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

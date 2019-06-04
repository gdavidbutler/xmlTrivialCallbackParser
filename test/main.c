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
      printf("/%.*s", tg->l, tg->s);
    printf("\n");
    break;
  case xmlTp_Ea:
    printf("A ");
    for (; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    if (nm)
      printf(":\"%.*s\"=", nm->l, nm->s);
    else
      printf(":");
    {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("\"%.*s\"(%d:%u)\n", vl->l, vl->s, i, vl->l);
      else
        printf("\"%.*s\"(%.*s)\n", vl->l, vl->s, i, d);
      free(d);
    }
    break;
  case xmlTp_Ec:
    printf("C ");
    for (; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    printf(":");
    {
      unsigned char *d;

      i = -1;
      if (!(d = malloc(vl->l))
       || (i = xmlDecodeBody(d, vl->l, vl->s, vl->l)) < 0
       || i > (int)vl->l)
        printf("(%.*s)(%d:%u)\n", vl->l, vl->s, i, vl->l);
      else
        printf("(%.*s)(%.*s)\n", vl->l, vl->s, i, d);
      free(d);
    }
    break;
  case xmlTp_Ee:
    printf("E ");
    for (; l; l--, tg++)
      printf("/%.*s", tg->l, tg->s);
    printf("\n");
    break;
  }
  return 0;
}

int
main(
  int argc
 ,char *argv[]
){
  static const unsigned char enc[] = "this is a test <of>, <![CDATA[<hello>]]> & ]]>. how did it do?";
  static const unsigned char uri[] = "http%3A%2F%2Ffoo.bar%23foo%3Ffoo%3Db%2Ba%2Br%26bar%3Df%20o%20o";
  static const unsigned char sur[] = "http://foo.bar#foo?foo=b+a+r&bar=f o o";
  static const char b64[] = "QmFzZTY0";
  static const unsigned char s64[] = "Base64";
  static const char hex[] = "486578";
  static const unsigned char shx[] = "Hex";
  xmlSt_t tg[64];
  int fd;
  int sz;
  unsigned char *bf;

  if (argc == 2) {
    if (!(bf = malloc(BUFSIZ)))
      return 2;
    if ((sz = xmlEncodeString(bf, BUFSIZ, enc, sizeof(enc) - 1)) > BUFSIZ)
      return 2;
    printf("xmlEncodeString(%s)\n->\n%.*s\n", enc, sz, bf);
    putchar('\n');
    if ((sz = xmlEncodeCdata(bf, BUFSIZ, enc, sizeof(enc) - 1)) > BUFSIZ)
      return 2;
    printf("xmlEncodeCdata(%s)\n->\n%.*s\n", enc, sz, bf);
    putchar('\n');
    if ((sz = xmlDecodeUri(bf, BUFSIZ, uri, sizeof(uri) - 1)) > BUFSIZ)
      return 2;
    printf("xmlDecodeUri(%s)\n->\n%.*s\n", uri, sz, bf);
    putchar('\n');
    if ((sz = xmlEncodeUri((char *)bf, BUFSIZ, sur, sizeof(sur) - 1)) > BUFSIZ)
      return 2;
    printf("xmlEncodeUri(%s)\n->\n%.*s\n", sur, sz, bf);
    putchar('\n');
    if ((sz = xmlDecodeBase64((unsigned char *)bf, BUFSIZ, b64, sizeof(b64) - 1)) > BUFSIZ)
      return 2;
    printf("xmlDecodeBase64(%s)\n->\n%.*s\n", b64, sz, bf);
    putchar('\n');
    if ((sz = xmlEncodeBase64((char *)bf, BUFSIZ, s64, sizeof(s64) - 1)) > BUFSIZ)
      return 2;
    printf("xmlEncodeBase64(%s)\n->\n%.*s\n", s64, sz, bf);
    putchar('\n');
    if ((sz = xmlDecodeHex((unsigned char *)bf, BUFSIZ, hex, sizeof(hex) - 1)) > BUFSIZ)
      return 2;
    printf("xmlDecodeHex(%s)\n->\n%.*s\n", hex, sz, bf);
    putchar('\n');
    if ((sz = xmlEncodeHex((char *)bf, BUFSIZ, shx, sizeof(shx) - 1)) > BUFSIZ)
      return 2;
    printf("xmlEncodeHex(%s)\n->\n%.*s\n", shx, sz, bf);
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
  bf = malloc(sz);
  if (read(fd, bf, sz) != sz) {
    fprintf(stderr, "%s: read fail on %s\n", argv[0], argv[2]);
    return 1;
  }
  close(fd);
  printf("%d %d\n", sz, xmlParse(atoi(argv[1]) ? cb : 0, sizeof(tg) / sizeof(tg[0]), tg, 1, bf, sz, 0));
  free(bf);
  return 0;
}

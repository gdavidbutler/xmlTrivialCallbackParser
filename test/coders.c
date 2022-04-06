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
#include "xml.h"

int
main(
){
  static const unsigned char enc[] = "this is a test <of>, <![CDATA[<hello>]]> & ]]>. how did it do?";
  static const unsigned char uri[] = "http%3A%2F%2Ffoo.bar%23foo%3Ffoo%3Db%2Ba%2Br%26bar%3Df%20o%20o";
  static const unsigned char sur[] = "http://foo.bar#foo?foo=b+a+r&bar=f o o";
  static const char b64[] = "QmFzZTY0";
  static const unsigned char s64[] = "Base64";
  static const char hex[] = "486578";
  static const unsigned char shx[] = "Hex";
  int sz;
  unsigned char *bf;

  if (!(bf = malloc(BUFSIZ)))
    return (2);
  if ((sz = xmlEncodeString(bf, BUFSIZ, enc, sizeof (enc) - 1)) > BUFSIZ)
    return (2);
  printf("xmlEncodeString(%s)\n->\n%.*s\n", enc, sz, bf);
  putchar('\n');
  if ((sz = xmlEncodeCdata(bf, BUFSIZ, enc, sizeof (enc) - 1)) > BUFSIZ)
    return (2);
  printf("xmlEncodeCdata(%s)\n->\n%.*s\n", enc, sz, bf);
  putchar('\n');
  if ((sz = xmlDecodeUri(bf, BUFSIZ, uri, sizeof (uri) - 1)) > BUFSIZ)
    return (2);
  printf("xmlDecodeUri(%s)\n->\n%.*s\n", uri, sz, bf);
  putchar('\n');
  if ((sz = xmlEncodeUri((char *)bf, BUFSIZ, sur, sizeof (sur) - 1)) > BUFSIZ)
    return (2);
  printf("xmlEncodeUri(%s)\n->\n%.*s\n", sur, sz, bf);
  putchar('\n');
  if ((sz = xmlDecodeBase64((unsigned char *)bf, BUFSIZ, b64, sizeof (b64) - 1)) > BUFSIZ)
    return (2);
  printf("xmlDecodeBase64(%s)\n->\n%.*s\n", b64, sz, bf);
  putchar('\n');
  if ((sz = xmlEncodeBase64((char *)bf, BUFSIZ, s64, sizeof (s64) - 1)) > BUFSIZ)
    return (2);
  printf("xmlEncodeBase64(%s)\n->\n%.*s\n", s64, sz, bf);
  putchar('\n');
  if ((sz = xmlDecodeHex((unsigned char *)bf, BUFSIZ, hex, sizeof (hex) - 1)) > BUFSIZ)
    return (2);
  printf("xmlDecodeHex(%s)\n->\n%.*s\n", hex, sz, bf);
  putchar('\n');
  if ((sz = xmlEncodeHex((char *)bf, BUFSIZ, shx, sizeof (shx) - 1)) > BUFSIZ)
    return (2);
  printf("xmlEncodeHex(%s)\n->\n%.*s\n", shx, sz, bf);
  free(bf);
  return (0);
}

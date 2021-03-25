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

#include "xml.h"

int
xmlParse(
  xmlCb_t c
 ,unsigned int m
 ,xmlSt_t *t
 ,const unsigned char *s
 ,unsigned int l
,void *v
){
  const unsigned char *sb; /* save original buffer offset */
  const unsigned char *cl; /* pointer to character after ':' if found */
  xmlSt_t nm;              /* attribute name */
  xmlSt_t vl;              /* value */
  long ns;                 /* non-space character seen in body */
  unsigned int tL;         /* current level */
  unsigned int ii;         /* in instruction */
  unsigned int is;         /* in section */

  if (!(sb = s))
    return (-1);
  ns = 0;
  tL = 0;
  ii = 0;
  is = 0;

end:
  cl = 0;
  nm.l = 0;
  vl.s = s;
  vl.o = 0;
  goto bgn;

err:
  l++, s--;
  goto rtn;

atrEq:
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  default:
    goto atrValNq;
  }
  goto rtn;

nlTg:
  if (c && ((nm.s = s), c(xmlTp_Ee, tL, t, 0, &nm, v)))
    goto rtn;
  tL--;
  for (; l--;) switch (*s++) {
  case '>':
    goto end;

  default:
    goto err;
  }
  goto rtn;

nlAtrVal:
  if (c && c(xmlTp_Ea, tL, t, 0, &nm, v))
    goto rtn;
  nm.l = 0;
  l++, s--;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (ii)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (ii && *s == '>') {
      ii = 0;
      goto nlTg;
    } else
      goto atr;

  case '<':
    goto err;

  case '>':
    if (ii)
      goto atr;
    else if (is) {
      is = 0;
      l++, s--;
      goto nlTg;
    } else
      goto end;

  case '[':
    if (is)
      goto atrValBr;
    else
      goto atr;

  default:
    goto atr;
  }
  goto rtn;

atrNm:
  nm.l = s - nm.s - 1;
  if (cl) {
    nm.o = cl - nm.s;
    cl = 0;
  } else
    nm.o = 0;
  l++, s--;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '<':
    goto err;

  case '=':
    goto atrEq;

  default:
    goto nlAtrVal;
  }
  goto rtn;

atr:
  nm.s = s - 1;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '=': case '>':
    goto atrNm;

  case ':':
    if (!cl)
      cl = s;
    break;

  case '<':
    goto err;

  default:
    break;
  }
  goto rtn;

atrVal:
  if (cl) {
    vl.o = cl - vl.s;
    cl = 0;
  } else
    vl.o = 0;
  if (c) {
    if (nm.l) {
      if (c(xmlTp_Ea, tL, t, &nm, &vl, v))
        goto rtn;
      nm.l = 0;
    } else if (c(xmlTp_Ea, tL, t, 0, &vl, v))
      goto rtn;
  }
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (ii)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (ii && *s == '>') {
      ii = 0;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (ii)
      goto atr;
    else if (is) {
      is = 0;
      l++, s--;
      goto nlTg;
    } else
      goto end;

  case '[':
    if (is)
      goto atrValBr;
    else
      goto atr;

  default:
    goto atr;
  }
  goto rtn;

atrValDq:
  vl.s = s;
  for (; l--;) switch (*s++) {
  case '"':
    vl.l = s - vl.s - 1;
    goto atrVal;

  case ':':
    if (!cl)
      cl = s;
    break;

  default:
    break;
  }
  goto rtn;

atrValSq:
  vl.s = s;
  for (; l--;) switch (*s++) {
  case '\'':
    vl.l = s - vl.s - 1;
    goto atrVal;

  case ':':
    if (!cl)
      cl = s;
    break;

  default:
    break;
  }
  goto rtn;

atrValNq:
  vl.s = s - 1;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '>':
    l++, s--;
    vl.l = s - vl.s;
    goto atrVal;

  case ':':
    if (!cl)
      cl = s;
    break;

  default:
    break;
  }
  goto rtn;

atrValBr:
  vl.s = s;
  is++;
  for (; l--;) switch (*s++) {
  case ':':
    if (!cl)
      cl = s;
    break;

  case '[':
    is++;
    break;

  case ']':
    if (--is == 1) {
      vl.l = s - vl.s - 1;
      goto atrVal;
    }

  default:
    break;
  }
  goto rtn;

eTgNm:
  (t + tL - 1)->l = s - (t + tL - 1)->s - 1;
  if (cl) {
    (t + tL - 1)->o = cl - (t + tL - 1)->s;
    cl = 0;
  } else
    (t + tL - 1)->o = 0;
  if (c && ((nm.s = s), c(xmlTp_Ee, tL, t, 0, &nm, v)))
    goto rtn;
  tL--;
  l++, s--;
  for (; l--;) switch (*s++) {
  case '>':
    goto end;

  default:
    goto err;
  }
  goto rtn;

eNm:
  if (!tL)
    goto err;
  (t + tL - 1)->s = s - 1;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '/': case '<':
    goto err;

  case ':':
    if (!cl)
      cl = s;
    break;

  case '>':
    goto eTgNm;

  default:
    break;
  }
  goto rtn;

eTg:
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  default:
    goto eNm;
  }
  goto rtn;

sTgNm:
  (t + tL)->l = s - (t + tL)->s - 1;
  if (cl) {
    (t + tL)->o = cl - (t + tL)->s;
    cl = 0;
  } else
    (t + tL)->o = 0;
  if ((t + tL)->l) {
    if (*(t + tL)->s == '?')
      ii = 1;
    else if (*(t + tL)->s == '!')
      is = 1;
  }
  tL++;
  if (c && ((nm.s = (t + tL - 1)->s - 1), c(xmlTp_Eb, tL, t, 0, &nm, v)))
    goto rtn;
  l++, s--;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (ii)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (ii && *s == '>') {
      ii = 0;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (ii)
      goto atr;
    else if (is) {
      is = 0;
      l++, s--;
      goto nlTg;
    } else
      goto end;

  case '[':
    if (is)
      goto atrValBr;
    else
      goto atr;

  default:
    goto atr;
  }
  goto rtn;

sNm:
  if (tL == m)
    goto rtn;
  (t + tL)->s = s - 1;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
    goto sTgNm;

  case ':':
    if (!cl)
      cl = s;
    break;

  case '/':
    if (ii)
      break;
    else
      goto sTgNm;

  case '?':
    if (ii && *s == '>') {
      ii = 0;
      goto sTgNm;
    } else
      break;

  case '<':
    goto err;

  case '>':
    if (ii)
      break;
    else
      goto sTgNm;

  default:
    break;
  }
  goto rtn;

sTg:
  vl.l = s - vl.s - 1;
  if (c && vl.l && c(xmlTp_Ec, tL, t, (void *)ns, &vl, v))
    goto rtn;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  case '!':
    if (l > 5
     && *(s + 0) == '-'
     && *(s + 1) == '-') {
      for (l--, s++; l; l--, s++)
        if (*(s + 0) == '-'
         && *(s + 1) == '-'
         && *(s + 2) == '>') {
          l -= 3, s += 3;
          goto end;
        }
      l--, s++;
      goto rtn;
    }
    goto sNm;

  case '/':
    goto eTg;

  default:
    goto sNm;
  }
  goto rtn;

bgn:
  ns = 0;
  for (; l--;) switch (*s++) {
  case '\t': case '\n': case '\r': case ' ':
    break;

  case '<':
    if (l > 3
     && *(s + 0) == '!'
     && *(s + 1) == '[') {
      for (l--, s++; l; l--, s++)
        if (*(s + 0) == ']'
         && *(s + 1) == ']'
         && *(s + 2) == '>') {
          l -= 3, s += 3;
          goto bgn;
        }
      l--;
      s++;
      goto rtn;
    }
    goto sTg;

  case '>':
    if (is) {
      is = 0;
      l++, s--;
      goto nlTg;
    } else
      break;

  default:
    if (!ns)
      ns = -1;
    break;
  }

  vl.l = s - vl.s;
  if (c && vl.l)
    c(xmlTp_Ec, tL, t, (void *)ns, &vl, v);

rtn:
  return (s - sb);
}

int
xmlDecodeBody(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case '<':
    if (ilen > 10
     && *(in + 1) == '!'
     && *(in + 2) == '['
     && *(in + 3) == 'C'
     && *(in + 4) == 'D'
     && *(in + 5) == 'A'
     && *(in + 6) == 'T'
     && *(in + 7) == 'A'
     && *(in + 8) == '[') {
      for (in += 9, ilen -= 8; ilen--; in++, len++) {
        if (*(in + 0) == ']'
         && *(in + 1) == ']'
         && *(in + 2) == '>') {
          in += 3, ilen -= 2;
          break;
        }
        if (olen > 0) {
          *out++ = *in;
          olen--;
        }
      }
    } else {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++, len++;
    }
    break;
  case '&':
    if (!(in++,ilen--)) goto err; else switch (*in) {
    case 'a':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 'm':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'p':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case ';':
            if (olen > 0) {
              *out++ = '&';
              olen--;
            }
            in++, len++;
            continue;
          default:
            goto err;
          }
        default:
          goto err;
        }
      case 'p':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'o':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case 's':
            if (!(in++,ilen--)) goto err; else switch (*in) {
            case ';':
              if (olen > 0) {
                *out++ = '\'';
                olen--;
              }
              in++, len++;
              continue;
            default:
              goto err;
            }
          default:
            goto err;
          }
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'g':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 't':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case ';':
          if (olen > 0) {
            *out++ = '>';
            olen--;
          }
          in++, len++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'l':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 't':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case ';':
          if (olen > 0) {
            *out++ = '<';
            olen--;
          }
          in++, len++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    case 'q':
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case 'u':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case 'o':
          if (!(in++,ilen--)) goto err; else switch (*in) {
          case 't':
            if (!(in++,ilen--)) goto err; else switch (*in) {
            case ';':
              if (olen > 0) {
                *out++ = '"';
                olen--;
              }
              in++, len++;
              continue;
            default:
              goto err;
            }
          default:
            goto err;
          }
        default:
          goto err;
        }
      default:
        goto err;
      }
    case '#':
      if (!(in++,ilen--)) goto err; else switch (*in) {
        unsigned long c;

      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        c = *in - '0';
        for (in++; ilen; in++, ilen--) switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c *= 10;
          c += *in - '0';
          break;
        case ';':
          goto enc;
        default:
          goto err;
        }
        goto err;
      case 'x':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c = *in - '0';
          goto nxtH;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          c = 10 + (*in - 'A');
          goto nxtH;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          c = 10 + (*in - 'a');
nxtH:
          for (in++; ilen; in++, ilen--) switch (*in) {
          case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            c *= 16;
            c += *in - '0';
            break;
          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            c *= 16;
            c += 10 + (*in - 'A');
            break;
          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            c *= 16;
            c += 10 + (*in - 'a');
            break;
          case ';':
            goto enc;
          default:
            goto err;
          }
          goto err;
enc:
          /* https://en.wikipedia.org/wiki/UTF-8 */
          if (!ilen--)
            goto err;
          if (c <= 0x7f) { /* 7 bits */
            if (olen > 0) {
              *out++ = c;
              olen--;
            }
            len++;
          } else if (c <= 0x7ff) { /* 11 bits */
            if (olen > 1) {
              *out++ = 0xc0 | (c >> 6);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 2;
            }
            len += 2;
          } else if (c <= 0xffff) { /* 16 bits */
            if (olen > 2) {
              *out++ = 0xe0 | (c >> 12);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 3;
            }
            len += 3;
          } else if (c <= 0x1fffff) { /* 21 bits */
            if (olen > 3) {
              *out++ = 0xf0 | (c >> 18);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 4;
            }
            len += 4;
          } else if (c <= 0x3ffffff) { /* 26 bits */
            if (olen > 4) {
              *out++ = 0xf8 | (c >> 24);
              *out++ = 0x80 | (c >> 18 & 0x3f);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 5;
            }
            len += 5;
          } else if (c <= 0x7fffffff) { /* 31 bits */
            if (olen > 5) {
              *out++ = 0xfc | (c >> 30);
              *out++ = 0x80 | (c >> 24 & 0x3f);
              *out++ = 0x80 | (c >> 18 & 0x3f);
              *out++ = 0x80 | (c >> 12 & 0x3f);
              *out++ = 0x80 | (c >> 6 & 0x3f);
              *out++ = 0x80 | (c & 0x3f);
              olen -= 6;
            }
            len += 6;
          } else
            goto err;
          in++;
          continue;
        default:
          goto err;
        }
      default:
        goto err;
      }
    default:
      goto err;
    }
  default:
    /* https://en.wikipedia.org/wiki/UTF-8 */
    if ((*in & 0x80) == 0x00) { /* 1 byte */
      if (olen > 0) {
        *out++ = *in++;
        olen--;
      } else
        in++;
      len++;
    } else if (ilen > 0
            && (*in & 0xe0) == 0xc0 /* 2 bytes */
            && (*(in + 1) & 0xc0) == 0x80) {
      if (olen > 1) {
        *out++ = *in++;
        *out++ = *in++;
        olen -= 2;
      } else
        in += 2;
      len += 2;
      ilen--;
    } else if (ilen > 1
            && (*in & 0xf0) == 0xe0 /* 3 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80) {
      if (olen > 2) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 3;
      } else
        in += 3;
      len += 3;
      ilen -= 2;
    } else if (ilen > 2
            && (*in & 0xf8) == 0xf0 /* 4 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80) {
      if (olen > 3) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 4;
      } else
        in += 4;
      len += 4;
      ilen -= 3;
    } else if (ilen > 3
            && (*in & 0xfc) == 0xf8 /* 5 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80) {
      if (olen > 4) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 5;
      } else
        in += 5;
      len += 5;
      ilen -= 4;
    } else if (ilen > 4
            && (*in & 0xfe) == 0xfc /* 6 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80
            && (*(in + 5) & 0xc0) == 0x80) {
      if (olen > 5) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 6;
      } else
        in += 6;
      len += 6;
      ilen -= 5;
    } else
      goto err;
    break;
  }
  return (len);
err:
  return (-1);
}

int
xmlEncodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
  case 0x08:                       case 0x0b: case 0x0c:            case 0x0e: case 0x0f:
  case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
  case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
                                                                               case 0x7f:
    return (-1);
    break;
  case '&':
    if (olen > 4) {
      *out++ = '&';
      *out++ = 'a';
      *out++ = 'm';
      *out++ = 'p';
      *out++ = ';';
      olen -= 5;
    }
    in++;
    len += 5;
    break;
  case '\'':
    if (olen > 5) {
      *out++ = '&';
      *out++ = 'a';
      *out++ = 'p';
      *out++ = 'o';
      *out++ = 's';
      *out++ = ';';
      olen -= 6;
    }
    in++;
    len += 6;
    break;
  case '>':
    if (olen > 3) {
      *out++ = '&';
      *out++ = 'g';
      *out++ = 't';
      *out++ = ';';
      olen -= 4;
    }
    in++;
    len += 4;
    break;
  case '<':
    if (olen > 3) {
      *out++ = '&';
      *out++ = 'l';
      *out++ = 't';
      *out++ = ';';
      olen -= 4;
    }
    in++;
    len += 4;
    break;
  case '"':
    if (olen > 5) {
      *out++ = '&';
      *out++ = 'q';
      *out++ = 'u';
      *out++ = 'o';
      *out++ = 't';
      *out++ = ';';
      olen -= 6;
    }
    in++;
    len += 6;
    break;
  default:
    /* https://en.wikipedia.org/wiki/UTF-8 */
    if ((*in & 0x80) == 0x00) { /* 1 byte */
      if (olen > 0) {
        *out++ = *in++;
        olen--;
      } else
        in++;
      len++;
    } else if (ilen > 0
            && (*in & 0xe0) == 0xc0 /* 2 bytes */
            && (*(in + 1) & 0xc0) == 0x80) {
      if (olen > 1) {
        *out++ = *in++;
        *out++ = *in++;
        olen -= 2;
      } else
        in += 2;
      len += 2;
      ilen--;
    } else if (ilen > 1
            && (*in & 0xf0) == 0xe0 /* 3 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80) {
      if (olen > 2) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 3;
      } else
        in += 3;
      len += 3;
      ilen -= 2;
    } else if (ilen > 2
            && (*in & 0xf8) == 0xf0 /* 4 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80) {
      if (olen > 3) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 4;
      } else
        in += 4;
      len += 4;
      ilen -= 3;
    } else if (ilen > 3
            && (*in & 0xfc) == 0xf8 /* 5 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80) {
      if (olen > 4) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 5;
      } else
        in += 5;
      len += 5;
      ilen -= 4;
    } else if (ilen > 4
            && (*in & 0xfe) == 0xfc /* 6 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80
            && (*(in + 5) & 0xc0) == 0x80) {
      if (olen > 5) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 6;
      } else
        in += 6;
      len += 6;
      ilen -= 5;
    } else
      return (-1);
    break;
  }
  return (len);
}

int
xmlEncodeCdata(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  static const char b[] = "<![CDATA[";
  static const char e[] = "]]>";
  int len;
  unsigned int i;

  len = 0;
  for (i = 0; i < sizeof (b) - 1; i++, len++)
    if (olen > 0) {
      *out++ = b[i];
      olen--;
    }
  for (; ilen--;) switch (*in) {
  case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
  case 0x08:                       case 0x0b: case 0x0c:            case 0x0e: case 0x0f:
  case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
  case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
                                                                               case 0x7f:
    return (-1);
    break;
  case ']':
    if (ilen > 1
     && *(in + 1) == ']'
     && *(in + 2) == '>') {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++, len++;
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++, len++;
      for (i = 0; i < sizeof (e) - 1; i++, len++)
        if (olen > 0) {
          *out++ = e[i];
          olen--;
        }
      for (i = 0; i < sizeof (b) - 1; i++, len++)
        if (olen > 0) {
          *out++ = b[i];
          olen--;
        }
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++, len++;
      ilen -= 2;
    } else {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++, len++;
    }
    break;
  default:
    /* https://en.wikipedia.org/wiki/UTF-8 */
    if ((*in & 0x80) == 0x00) { /* 1 byte */
      if (olen > 0) {
        *out++ = *in++;
        olen--;
      } else
        in++;
      len++;
    } else if (ilen > 0
            && (*in & 0xe0) == 0xc0 /* 2 bytes */
            && (*(in + 1) & 0xc0) == 0x80) {
      if (olen > 1) {
        *out++ = *in++;
        *out++ = *in++;
        olen -= 2;
      } else
        in += 2;
      len += 2;
      ilen--;
    } else if (ilen > 1
            && (*in & 0xf0) == 0xe0 /* 3 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80) {
      if (olen > 2) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 3;
      } else
        in += 3;
      len += 3;
      ilen -= 2;
    } else if (ilen > 2
            && (*in & 0xf8) == 0xf0 /* 4 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80) {
      if (olen > 3) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 4;
      } else
        in += 4;
      len += 4;
      ilen -= 3;
    } else if (ilen > 3
            && (*in & 0xfc) == 0xf8 /* 5 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80) {
      if (olen > 4) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 5;
      } else
        in += 5;
      len += 5;
      ilen -= 4;
    } else if (ilen > 4
            && (*in & 0xfe) == 0xfc /* 6 bytes */
            && (*(in + 1) & 0xc0) == 0x80
            && (*(in + 2) & 0xc0) == 0x80
            && (*(in + 3) & 0xc0) == 0x80
            && (*(in + 4) & 0xc0) == 0x80
            && (*(in + 5) & 0xc0) == 0x80) {
      if (olen > 5) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
        olen -= 6;
      } else
        in += 6;
      len += 6;
      ilen -= 5;
    } else
      return (-1);
    break;
  }
  for (i = 0; i < sizeof (e) - 1; i++, len++)
    if (olen > 0) {
      *out++ = e[i];
      olen--;
    }
  return (len);
}

int
xmlDecodeUri(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  int len;
  unsigned char c;

  len = 0;
  for (; ilen--;) switch (*in) {
  case '%':
    if (!(in++,ilen--)) goto err; else switch (*in) {
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      c = *in - '0';
      goto nxtH;
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      c = 10 + (*in - 'A');
      goto nxtH;
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      c = 10 + (*in - 'a');
nxtH:
      if (!(in++,ilen--)) goto err; else switch (*in) {
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        c *= 16;
        c += *in - '0';
        break;
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        c *= 16;
        c += 10 + (*in - 'A');
        break;
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        c *= 16;
        c += 10 + (*in - 'a');
        break;
      default:
        goto err;
      }
      if (olen > 0) {
        *out++ = c;
        olen--;
      }
      in++, len++;
      break;
    default:
      goto err;
    }
    break;
  default:
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++, len++;
    break;
  }
  return (len);
err:
  return (-1);
}

int
xmlEncodeUri(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  static const char hex[] = "0123456789ABCDEF";
  int len;

  len = 0;
  for (; ilen--;) switch (*in) { case '-': case '.':
  case '0': case'1': case'2': case'3': case'4': case'5': case'6': case'7': case'8': case'9':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
  case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
  case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
  case '~':
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++, len++;
    break;
  default:
    if (olen > 2) {
      *out++ = '%';
      *out++ = hex[*in >> 4];
      *out++ = hex[*in & 0x0f];
      olen -= 3;
    }
    in++;
    len += 3;
    break;
  }
  return (len);
}

int
xmlDecodeBase64(
  unsigned char *out
 ,unsigned int olen
 ,const char *in
 ,unsigned int ilen
){
  static const unsigned char b64[] = {
    66, 66, 66, 66,  66, 66, 66, 66,  66, 64, 64, 66,  66, 64, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    64, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 62,  66, 66, 66, 63,
    52, 53, 54, 55,  56, 57, 58, 59,  60, 61, 66, 66,  66, 65, 66, 66,
    66,  0,  1,  2,   3,  4,  5,  6,   7,  8,  9, 10,  11, 12, 13, 14,
    15, 16, 17, 18,  19, 20, 21, 22,  23, 24, 25, 66,  66, 66, 66, 66,
    66, 26, 27, 28,  29, 30, 31, 32,  33, 34, 35, 36,  37, 38, 39, 40,
    41, 42, 43, 44,  45, 46, 47, 48,  49, 50, 51, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,
    66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66,  66, 66, 66, 66
  };
  unsigned long buf;
  int len;

  buf = 1;
  len = 0;
  while (ilen-- > 0) {
    unsigned char c;

    switch ((c = b64[*(unsigned char*)in++])) {
    case 66: /* invalid */
      return (-1);
    case 64: /* whitespace */
      continue;
    case 65: /* pad */
      ilen = 0;
      break;
    default:
      buf = buf << 6 | c;
      if (buf & 0x1000000) {
        if (olen >= 3) {
          *out++ = buf >> 16;
          *out++ = buf >> 8;
          *out++ = buf;
          olen -= 3;
        }
        len += 3;
        buf = 1;
      }
      break;
    }
  }
  if (buf & 0x40000) {
    if (olen >= 2) {
      *out++ = buf >> 10;
      *out++ = buf >> 2;
    }
    len += 2;
  } else if (buf & 0x1000) {
    if (olen >= 1)
      *out++ = buf >> 4;
    len++;
  }
  return (len);
}

int
xmlEncodeBase64(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  static const char b64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int len;

  for (len = 0; ilen >= 3; in += 3, ilen -= 3, len += 4) {
    if (olen >= 4) {
      *out++ = b64[in[0] >> 2];
      *out++ = b64[((in[0] << 4) & 0x30) | (in[1] >> 4)];
      *out++ = b64[((in[1] << 2) & 0x3c) | (in[2] >> 6)];
      *out++ = b64[in[2] & 0x3f];
      olen -= 4;
    }
  }
  if (ilen) {
    if (olen >= 4) {
      unsigned char frag;

      *out++ = b64[in[0] >> 2];
      frag = (in[0] << 4) & 0x30;
      if (ilen > 1)
          frag |= in[1] >> 4;
      *out++ = b64[frag];
      *out++ = (ilen > 1) ? b64[(in[1] << 2) & 0x3c] : '=';
      *out++ = '=';
    }
    len += 4;
  }
  return (len);
}

int
xmlDecodeHex(
  unsigned char *out
 ,unsigned int olen
 ,const char *in
 ,unsigned int ilen
){
  static const unsigned char hex[] = {
    17, 17, 17, 17,  17, 17, 17, 17,  17, 16, 16, 17,  17, 16, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    16, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
     0,  1,  2,  3,   4,  5,  6,  7,   8,  9, 17, 17,  17, 17, 17, 17,
    17, 10, 11, 12,  13, 14, 15, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 10, 11, 12,  13, 14, 15, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,
    17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17,  17, 17, 17, 17
  };
  unsigned long buf;
  int len;

  buf = 1;
  len = 0;
  while (ilen-- > 0) {
    unsigned char c;

    switch ((c = hex[*(unsigned char*)in++])) {
    case 17: /* invalid */
      return (-1);
    case 16: /* whitespace */
      continue;
    default:
      buf = buf << 4 | c;
      if (buf & 0x100) {
        if (olen > 0) {
          *out++ = buf;
          olen--;
        }
        len++;
        buf = 1;
      }
      break;
    }
  }
  if (buf != 1)
    return (-1);
  return (len);
}

int
xmlEncodeHex(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
){
  static const char hex[] =
   "0123456789ABCDEF";
  int len;

  for (len = 0; ilen > 0; in++, ilen--, len += 2) {
    if (olen > 1) {
      *out++ = hex[*in >> 4];
      *out++ = hex[*in & 0x0f];
      olen -= 2;
    }
  }
  return (len);
}

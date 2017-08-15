/*
 * Copyright 2017 G. David Butler. All rights reserved.
 * License: https://github.com/gdavidbutler/xmlTrivialCallbackParser/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include "xml.h"

int
xmlParse(
  xmlCb_t cb
 ,const char *s
,void *v
){
  const char *b;
  unsigned int tgM; /* maximum level for allocation */
  unsigned int tgL; /* current level */
  unsigned int tgD; /* gone deeper? for body */
  xmlSt_t *tg;
  xmlSt_t nm;
  xmlSt_t vl;
  int inXml;
  int inDoctype;
  char ers[32];

  if (!(b = s))
    return -1;
  tgD = tgL = tgM = 0;
  tg = 0;
  inXml = 0;
  inDoctype = 0;

tgEnd:
  vl.s = s;
  goto bgn;

err:
  vl.l = 1;
  vl.s = s - 1;
  nm.l = snprintf(ers, sizeof(ers), "Error@%zd", vl.s - b);
  nm.s = ers;
  if (cb)
    cb(xmlTp_Er, tgL, tg, &nm, &vl, v);
  goto rtn;

atrEq:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  default:
    goto err;
  }

nlTg:
  vl.l = 0;
  if (cb)
    cb(xmlTp_Ee, tgL, tg, 0, &vl, v);
  if (tgL)
    tgL--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '>':
    goto tgEnd;

  default:
    goto err;
  }

nlAtrVal:
  vl.l = 0;
  if (cb)
    cb(xmlTp_Ea, tgL, tg, &nm, &vl, v);
  nm.l = 0;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (inXml)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (inXml) {
      inXml--;
      goto nlTg;
    } else
      goto atr;

  case '<':
    goto err;

  case '>':
    if (inDoctype) {
      inDoctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (inDoctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

atrNm:
  nm.l = s - nm.s - 1;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '<':
    goto err;

  case '=':
    goto atrEq;

  default:
    goto nlAtrVal;
  }

atr:
  nm.s = s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '/': case '?': case '=': case '>':
    goto atrNm;

  case '<':
    goto err;

  default:
    break;
  }

atrVal:
  vl.l = s - vl.s - 1;
  if (cb) {
    if (nm.l)
      cb(xmlTp_Ea, tgL, tg, &nm, &vl, v);
    else
      cb(xmlTp_Ea, tgL, tg, &vl, &nm, v);
  }
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (inXml)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (inXml) {
      inXml--;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (inDoctype) {
      inDoctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (inDoctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

atrValDq:
  vl.s = s;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '"':
    goto atrVal;

  case '<': case '>':
    goto err;

  default:
    break;
  }

atrValSq:
  vl.s = s;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\'':
    goto atrVal;

  case '<': case '>':
    goto err;

  default:
    break;
  }

eTgNm:
  if (tgL)
    (tg + tgL - 1)->l = s - (tg + tgL - 1)->s - 1;
  else {
    (tg + tgL)->l = s - (tg + tgL)->s - 1;
    tgL++;
  }
  if (tgL <= tgD)
    vl.l = 0;
  if (cb)
    cb(xmlTp_Ee, tgL, tg, 0, &vl, v);
  if (tgL)
    tgL--;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '>':
    goto tgEnd;

  default:
    goto err;
  }

eNm:
  if (tgL)
    (tg + tgL - 1)->s = s - 1;
  else {
    if (tgL == tgM) {
      void *t;

      if (!(t = realloc(tg, (tgM + 1) * sizeof(*tg))))
        goto rtn;
      tg = t;
      tgM++;
    }
    (tg + tgL)->s = s - 1;
  }
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '/': case '<':
    goto err;

  case '>':
    goto eTgNm;

  default:
    break;
  }

eTg:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  default:
    goto eNm;
  }

sTgNm:
  (tg + tgL)->l = s - (tg + tgL)->s - 1;
  if ((tg + tgL)->l == 4
   && *((tg + tgL)->s + 0) == '?'
   && *((tg + tgL)->s + 1) == 'x'
   && *((tg + tgL)->s + 2) == 'm'
   && *((tg + tgL)->s + 3) == 'l')
    inXml = 1;
  else if ((tg + tgL)->l == 8
   && *((tg + tgL)->s + 0) == '!'
   && *((tg + tgL)->s + 1) == 'D'
   && *((tg + tgL)->s + 2) == 'O'
   && *((tg + tgL)->s + 3) == 'C'
   && *((tg + tgL)->s + 4) == 'T'
   && *((tg + tgL)->s + 5) == 'Y'
   && *((tg + tgL)->s + 6) == 'P'
   && *((tg + tgL)->s + 7) == 'E')
    inDoctype = 1;
  tgD = tgL++;
  if (cb)
    cb(xmlTp_Eb, tgL, tg, 0, 0, v);
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
    break;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
    if (inXml)
      goto atr;
    else
      goto nlTg;

  case '?':
    if (inXml) {
      inXml--;
      goto nlTg;
    } else
      goto atr;

  case '<': case '=':
    goto err;

  case '>':
    if (inDoctype) {
      inDoctype--;
      s--;
      goto nlTg;
    } else
      goto tgEnd;

  case '[':
    if (inDoctype)
      goto tgEnd;
    else
      goto atr;

  default:
    goto atr;
  }

sNm:
  if (tgL == tgM) {
    void *t;

    if (!(t = realloc(tg, (tgM + 1) * sizeof(*tg))))
      goto rtn;
    tg = t;
    tgM++;
  }
  (tg + tgL)->s = s - 1;
  if (inDoctype)
    inDoctype++;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'': case '>':
    goto sTgNm;

  case '/':
    if (inXml)
      break;
    else
      goto sTgNm;

  case '?':
    if (inXml) {
      inXml--;
      goto sTgNm;
    } else
      break;

  case '<':
    goto err;

  default:
    break;
  }

sTg:
  vl.l = s - vl.s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '\t': case '\n': case '\r': case ' ':
  case '"': case '\'':
  case '<': case '>':
    goto err;

  case '!':
    if (*(s + 0) == '-'
     && *(s + 1) == '-'
     && (*(s + 2) == ' ' || *(s + 2) == '\n' || *(s + 2) == '\r')) {
      for (s += 2; *s; s++)
        if ((*(s + 0) == ' ' || *(s + 0) == '\n' || *(s + 0) == '\r')
         && *(s + 1) == '-'
         && *(s + 2) == '-'
         && *(s + 3) == '>') {
          s += 4;
          goto tgEnd;
        }
      s++;
      goto rtn;
    }
    goto sNm;

  case '/':
    goto eTg;

  default:
    goto sNm;
  }

bgn:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '<':
    if (*(s + 0) == '!'
     && *(s + 1) == '['
     && *(s + 2) == 'C'
     && *(s + 3) == 'D'
     && *(s + 4) == 'A'
     && *(s + 5) == 'T'
     && *(s + 6) == 'A'
     && *(s + 7) == '[') {
      for (s += 7; *s; s++)
        if (*(s + 0) == ']'
         && *(s + 1) == ']'
         && *(s + 2) == '>') {
          s += 3;
          goto bgn;
        }
      s++;
      goto rtn;
    }
    goto sTg;

  case '>':
    if (inDoctype) {
      inDoctype--;
      s--;
      goto nlTg;
    } else
      break;

  default:
    break;
  }

rtn:
  free(tg);
  return s - b - 1;
}

int
xmlDecodeBody(
  char *out
 ,int olen
 ,const char *in
 ,int ilen
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
      for (in += 9, ilen -= 9; ilen; ilen--, in++, len++)
        if (olen > 0) {
          *out++ = *in;
          olen--;
        }
        if (*(in + 0) == ']'
         && *(in + 1) == ']'
         && *(in + 2) == '>') {
          in += 3, ilen -= 2;
          break;
        }
    } else {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
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
            in++;
            len++;
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
              in++;
              len++;
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
          in++;
          len++;
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
          in++;
          len++;
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
              in++;
              len++;
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
        int c;

      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        c = *in - '0';
        for (in++; ilen; in++, ilen--) switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c *= 10;
          c += *in - '0';
          break;
        case ';':
          goto endD;
        default:
          goto err;
        }
endD:
        if (!ilen--)
          goto err;
        if (olen > 0) {
          *out++ = c;
          olen--;
        }
        in++;
        len++;
        continue;
      case 'x':
        if (!(in++,ilen--)) goto err; else switch (*in) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
          c = *in - '0';
          goto bgnH;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
          c = 10 + (*in - 'A');
          goto bgnH;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          c = 10 + (*in - 'a');
bgnH:
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
            goto endH;
          default:
            goto err;
          }
endH:
          if (!ilen--)
            goto err;
          if (olen > 0) {
            *out++ = c;
            olen--;
          }
          in++;
          len++;
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
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++;
    len++;
    break;
  }
  return len;
err:
  return -1;
}

int
xmlEncodeString(
  char *out
 ,int olen
 ,const char *in
 ,int ilen
){
  int len;

  len = 0;
  for (; ilen--;) switch (*in) {
  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
  case 11: case 12: case 14: case 15: case 16: case 17: case 18: case 19:
  default:
    return -1;
    break;
  case '\t': case '\n': case '\r':
  case ' ': case '!': case '#': case '$': case '%': case'(': case')': case'*': case'+': case',': case'-': case'.': case'/':
  case '0': case'1': case'2': case'3': case'4': case'5': case'6': case'7': case'8': case'9':
  case ':': case';': case'=': case'?': case'@':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
  case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '[': case '\\': case ']': case '^': case '_': case '`':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
  case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
  case '~':
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++;
    len++;
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
  }
  return len;
}

int
xmlEncodeCdata(
  char *out
 ,int olen
 ,const char *in
 ,int ilen
){
  static const char b[] = "<![CDATA[";
  static const char e[] = "]]>";
  int len;
  unsigned int i;

  len = 0;
  for (i = 0; i < sizeof(b) - 1; i++, len++)
    if (olen > 0) {
      *out++ = b[i];
      olen--;
    }
  for (; ilen--;) switch (*in) {
  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
  case 11: case 12: case 14: case 15: case 16: case 17: case 18: case 19:
  default:
    return -1;
    break;
  case '\t': case '\n': case '\r':
  case ' ': case '!': case '"': case '#': case '$': case '%': case '&': case '\'':
  case '(': case ')': case '*': case '+': case ',': case '-': case '.': case '/':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
  case ':': case ';': case '<': case'=': case '>': case'?': case'@':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
  case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '[': case '\\': case '^': case '_': case '`':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
  case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
  case '~':
    if (olen > 0) {
      *out++ = *in;
      olen--;
    }
    in++;
    len++;
    break;
  case ']':
    if (ilen > 1
     && *(in + 1) == ']'
     && *(in + 2) == '>') {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
      for (i = 0; i < sizeof(e) - 1; i++, len++)
        if (olen > 0) {
          *out++ = e[i];
          olen--;
        }
      for (i = 0; i < sizeof(b) - 1; i++, len++)
        if (olen > 0) {
          *out++ = b[i];
          olen--;
        }
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
      ilen -= 2;
    } else {
      if (olen > 0) {
        *out++ = *in;
        olen--;
      }
      in++;
      len++;
    }
    break;
  }
  for (i = 0; i < sizeof(e) - 1; i++, len++)
    if (olen > 0) {
      *out++ = e[i];
      olen--;
    }
  return len;
}

int
xmlDecodeBase64(
  unsigned char *out
 ,int olen
 ,char const *in
 ,int ilen
){
  static unsigned char const b64[] = {
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
      return -1;
      break;
    case 64: /* whitespace */
      continue;
      break;
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
  return len;
}

int
xmlEncodeBase64(
  char *out
 ,int olen
 ,unsigned char const *in
 ,int ilen
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
  return len;
}

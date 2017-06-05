/*
 * Copyright 2017 G. David Butler. All rights reserved.
 * License: https://github.com/gdavidbutler/xmlTrivialCallbackParser/blob/master/LICENSE
 */

#include <stdlib.h>
#include "xml.h"

int
xmlParse(
  xmlCb cb
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

  if (!(b = s))
    return -1;
  tgD = tgL = tgM = 0;
  tg = 0;

tgEnd:
  vl.s = s;
  goto bgn;

err:
  nm.s = "Error!";
  nm.l = 6;
  vl.s = s - 1;
  vl.l = 1;
  if (cb)
    cb(xmlTp_Er, tgL, tg, &nm, &vl, v);
  goto rtn;

atrEq:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
    break;

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

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
  case '?':
    goto nlTg;

  case '>':
    goto tgEnd;

  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto atr;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
    break;

  default:
    goto err;
  }

atrNm:
  nm.l = s - nm.s - 1;
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '=':
    goto atrEq;

  case '"':
  case '\'':
  case '/':
  case '>':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto nlAtrVal;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
    break;

  default:
    goto err;
  }

atr:
  nm.s = s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '-':
  case '.':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
  case ':':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    break;

  default:
    goto atrNm;
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

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
  case '?':
    goto nlTg;

  case '>':
    goto tgEnd;

  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto atr;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
    break;

  default:
    goto err;
  }

atrValDq:
  vl.s = s;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '"':
    goto atrVal;

            case   1: case   2: case   3: case   4: case   5: case   6: case   7: case   8: case   9:
  case  10: case  11: case  12: case  13: case  14: case  15: case  16: case  17: case  18: case  19:
  case  20: case  21: case  22: case  23: case  24: case  25: case  26: case  27: case  28: case  29:
  case  30: case  31:
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

            case   1: case   2: case   3: case   4: case   5: case   6: case   7: case   8: case   9:
  case  10: case  11: case  12: case  13: case  14: case  15: case  16: case  17: case  18: case  19:
  case  20: case  21: case  22: case  23: case  24: case  25: case  26: case  27: case  28: case  29:
  case  30: case  31:
    goto err;

  default:
    break;
  }

eTgNm:
  (tg + tgL - 1)->l = s - (tg + tgL - 1)->s - 1;
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
  (tg + tgL - 1)->s = s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '-':
  case '.':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
  case ':':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    break;

  default:
    goto eTgNm;
  }

eTg:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto eNm;

  default:
    goto err;
  }

sTgNm:
  (tg + tgL)->l = s - (tg + tgL)->s - 1;
  tgD = tgL++;
  if (cb)
    cb(xmlTp_Eb, tgL, tg, 0, 0, v);
  s--;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '"':
    goto atrValDq;

  case '\'':
    goto atrValSq;

  case '/':
  case '?':
    goto nlTg;

  case '>':
    goto tgEnd;

  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto atr;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
    break;

  default:
    goto err;
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
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '-':
  case '.':
  case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
  case ':':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    break;

  default:
    goto sTgNm;
  }

sTg:
  vl.l = s - vl.s - 1;
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '/':
    goto eTg;

  case '!':
    if (*(s + 0) == '-'
     && *(s + 1) == '-'
     && (*(s + 2) == ' ' || *(s + 2) == '\n' || *(s + 2) == '\r')) {
      for (s += 2; *s; s++)
        if (*(s + 0) == ' '
         && *(s + 1) == '-'
         && *(s + 2) == '-'
         && *(s + 3) == '>') {
          s += 4;
          goto tgEnd;
        }
      s++;
      goto rtn;
    }
  case '?':
  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
  case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
  case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
  case '_':
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
  case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
  case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    goto sNm;

  default:
    goto err;
  }

bgn:
  for (;;) switch (*s++) {
  case '\0':
    goto rtn;

  case '<':
    goto sTg;

  case '\t':
  case '\n':
  case '\r':
  case ' ':
                                case  33: case  34: case  35: case  36: case  37: case  38: case  39:
  case  40: case  41: case  42: case  43: case  44: case  45: case  46: case  47: case  48: case  49:
  case  50: case  51: case  52: case  53: case  54: case  55: case  56: case  57: case  58: case  59:
            case  61: case  62: case  63: case  64: case  65: case  66: case  67: case  68: case  69:
  case  70: case  71: case  72: case  73: case  74: case  75: case  76: case  77: case  78: case  79:
  case  80: case  81: case  82: case  83: case  84: case  85: case  86: case  87: case  88: case  89:
  case  90: case  91: case  92: case  93: case  94: case  95: case  96: case  97: case  98: case  99:
  case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109:
  case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118: case 119:
  case 120: case 121: case 122: case 123: case 124: case 125: case 126:
    break;

  default:
    goto err;
  }

rtn:
  free(tg);
  return s - b - 1;
}

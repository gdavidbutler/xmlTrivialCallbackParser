/*
 * Copyright 2017 G. David Butler. All rights reserved.
 * License: https://github.com/gdavidbutler/xmlTrivialCallbackParser/blob/master/LICENSE
 */

/* callback event types */
typedef enum {
  xmlTp_Eb /* element begin */
 ,xmlTp_Ea /* element attribute */
 ,xmlTp_Ee /* element end */
 ,xmlTp_Er /* syntax error */
} xmlTp_t;

/* pointers referencing fragments of constant buffer must include length */
typedef struct {
  const char *s;
  unsigned int l;
} xmlSt_t;

/* prototype of a callback function */
typedef void (*xmlCb_t)(
  xmlTp_t
 ,unsigned int numberOfElementTagNames
 ,const xmlSt_t *elementTagName
 ,const xmlSt_t *elementAttributeName
 ,const xmlSt_t *EeBodyOrEaValue
 ,void *userContext
);

/* return -1 on null buf else offset of last char parsed */
int xmlParse(xmlCb_t, const char *buf, void *userContext);

/* return -1 on null dest or source else offset of last char decoded */
/* dest must be one char longer than length */
/* if return is not -1, then dest is '\0' terminated */
int xmlDecodeBody(char *dest, const char *source, unsigned int length);

/* return 0 on out of memory or invalid string characters */
/* returns allocated '\0' terminated buffer that must be free()'d */
char *xmlEncodeString(const char *source, unsigned int length);

/* return 0 on out of memory or invalid string characters */
/* returns allocated '\0' terminated buffer that must be free()'d */
char *xmlEncodeCdata(const char *source, unsigned int length);

/* For <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"> */

/* <foo type="xs:base64Binary"></foo> */

/* the minimum length of the out buffer */
#define xmlDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* returns the number of characters put in out */
int xmlDecodeBase64(unsigned char *out, int outl, const char *in, int inl);

/* the minimum length of the out buffer */
#define xmlEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* returns the number of characters put in out */
int xmlEncodeBase64(char *out, int outl, const unsigned char *in, int inl);

/* <foo type="xs:hexBinary">stuff</foo> */
/* TODO
int xmlDecodeHex (ignore whitespace \t\n\r)
char *xmlEncodeHex (only uppercase)
*/

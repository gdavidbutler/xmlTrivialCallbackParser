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

/* return -1 on error else offset of last char parsed */
/* buf must be null terminated */
int xmlParse(xmlCb_t, const char *buf, void *userContext);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBody(char *out, int olen, const char *in, int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeString(char *out, int olen, const char *in, int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeCdata(char *out, int olen, const char *in, int ilen);

/* For <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"> */

/* <foo type="xs:base64Binary"></foo> */

/* estimated length of needed out buffer */
#define xmlDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBase64(unsigned char *out, int olen, const char *in, int ilen);

/* estimated length of needed out buffer */
#define xmlEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeBase64(char *out, int olen, const unsigned char *in, int ilen);

/* <foo type="xs:hexBinary"></foo> */

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeHex(unsigned char *out, int olen, const char *in, int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeHex(char *out, int olen, const unsigned char *in, int ilen);

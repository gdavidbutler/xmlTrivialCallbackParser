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
  const unsigned char *s;
  unsigned int l;
} xmlSt_t;

/* prototype of a callback function */
/* returns 0 on success else aborts parse */
typedef int (*xmlCb_t)(
  xmlTp_t
 ,unsigned int numberOfElementTagNames
 ,const xmlSt_t *elementTagName
 ,const xmlSt_t *elementAttributeName
 ,const xmlSt_t *EeBodyOrEaValue
 ,void *userContext
);

/* return -1 on error else offset of last char parsed */
/* provide a tag buffer large enough for the deepest level of nesting supported */
int xmlParse(xmlCb_t, unsigned int numberOfElementTagBuf, xmlSt_t *elementTagBuf, const unsigned char *xml, unsigned int xlen, void *userContext);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBody(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeString(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeCdata(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* For <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"> */

/* <foo type="xs:anyURI"></foo> */

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeUri(unsigned char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeUri(char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* <foo type="xs:base64Binary"></foo> */

/* estimated length of needed out buffer */
#define xmlDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBase64(unsigned char *out, unsigned int olen, const char *in, unsigned int ilen);

/* estimated length of needed out buffer */
#define xmlEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeBase64(char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

/* <foo type="xs:hexBinary"></foo> */

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeHex(unsigned char *out, unsigned int olen, const char *in, unsigned int ilen);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeHex(char *out, unsigned int olen, const unsigned char *in, unsigned int ilen);

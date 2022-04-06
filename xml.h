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

/* xml "string" (fragment) is not \0 terminated */
typedef struct {
  const unsigned char *s;
  unsigned int l; /* entire length */
  unsigned int o; /* offset of local name (character after the first ':', if any) */
} xmlSt_t;

/* callback event types */
typedef enum {
  xmlTp_Eb /* element begin     - elementAttributeName is NULL and ecBodyOrEaValue is '<' (l is 0) */
 ,xmlTp_Ea /* element attribute - elementAttributeName is NULL if the value has no name */
 ,xmlTp_Ec /* element content   - elementAttributeName is NULL if ecBodyOrEaValue is only whitespace */
 ,xmlTp_Ee /* element end       - elementAttributeName is NULL and ecBodyOrEaValue is '>' (l is 0) */
} xmlTp_t;

/* prototype of callback function */
/* returns 0 on success else aborts parse */
typedef int (*xmlCb_t)(
  xmlTp_t
 ,unsigned int numberOfElementTagNames
 ,const xmlSt_t *elementTagName
 ,const xmlSt_t *elementAttributeName
 ,const xmlSt_t *ecBodyOrEaValue
 ,void *userContext
);

/* return -1 on error else offset of last char parsed */
/* provide a tag buffer large enough for the deepest level of nesting supported */
int xmlParse(
  xmlCb_t
 ,unsigned int numberOfElementTagBuf
 ,xmlSt_t *elementTagBuf
 ,const unsigned char *xml
 ,unsigned int xlen
 ,void *userContext
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBody(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeString(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeCdata(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* For <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"> */

/* <foo type="xs:anyURI"></foo> */

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeUri(
  unsigned char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeUri(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* <foo type="xs:base64Binary"></foo> */

/* estimated length of needed out buffer */
#define xmlDecodeBase64Need(inl) (((inl + 3) / 4) * 3)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeBase64(
  unsigned char *out
 ,unsigned int olen
 ,const char *in
 ,unsigned int ilen
);

/* estimated length of needed out buffer */
#define xmlEncodeBase64Need(inl) (((inl + 2) / 3) * 4)

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeBase64(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* <foo type="xs:hexBinary"></foo> */

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlDecodeHex(
  unsigned char *out
 ,unsigned int olen
 ,const char *in
 ,unsigned int ilen
);

/* return -1 on error else the length of out */
/* if length returned is more than length provided, allocate needed memory and retry */
int xmlEncodeHex(
  char *out
 ,unsigned int olen
 ,const unsigned char *in
 ,unsigned int ilen
);

/* XML document node */
typedef struct xmlNode {
  struct xmlNode *parent;
  struct {
    xmlSt_t name;
    xmlSt_t value;
  } *attribute;             /* array of attributes */
  struct xmlNode **node;    /* array of nodes */
  const unsigned char *beg; /* pointer to XML element begin '<' (0=content) */
  const unsigned char *end; /* pointer to XML element end '>' (0=content) */
  void *user;               /* user data (not used by xmlNode) */
  xmlSt_t value;            /* content or element name */
  unsigned int attributeN;  /* number of attributes */
  unsigned int nodeN;       /* number of nodes */
  unsigned int nodeW;       /* walk node */
} xmlNode_t;

/* parse an XML document of len with(out) white bodies into an allocated XML node */
/* return -1 on error else offset of last char parsed */
/* the node is allocated with all that was parseable */
int
xml2node(
  void *(*realloc)(void *, unsigned long)
 ,xmlNode_t *node
 ,xmlSt_t *elementTagBuf
 ,const unsigned char *xml
 ,unsigned int numberOfElementTagBuf
 ,unsigned int xlen
 ,int whiteBody
);

/* xmlNode walk visit types */
typedef enum {
  xmlNodeVisitPreorder
 ,xmlNodeVisitInorder
 ,xmlNodeVisitPostorder
 ,xmlNodeVisitLeaf
} xmlNodeVisit_t;

/* walk a xmlNode calling action on node with closure */
void
xmlNodeWalk(
  xmlNode_t *n
 ,void (*action)(xmlNode_t *node, unsigned int depth, xmlNodeVisit_t visit, void *closure)
 ,void *closure
);

/* deallocate an xmlNode tree */
void
xmlNodeFree(
  void (*free)(void *)
 ,xmlNode_t *node
);

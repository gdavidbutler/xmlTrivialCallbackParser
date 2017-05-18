typedef enum {
  xmlTp_Eb /* element begin */
 ,xmlTp_At /* element attribute */
 ,xmlTp_Ee /* element end */
 ,xmlTp_Er /* syntax error */
} xmlTp_t;

typedef struct {
  const char* s;
  int l;
} xmlSt_t;

typedef void (*xmlCb)(xmlTp_t, const xmlSt_t* elementTag, const xmlSt_t* attributeName, const xmlSt_t* AtValueOrEeBody, void* userContext);

/* return -1 on null buf or offset of last char parsed */
int xmlParse(xmlCb, const char *buf, void *userContext);

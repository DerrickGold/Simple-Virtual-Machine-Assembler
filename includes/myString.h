#ifndef _MYSTRING_H_
#define _MYSTRING_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef struct String_t {
	char *buf;
	int len;
}String_t;


extern String_t *String(char *text);
extern void String_destroy(String_t *str);
extern char *String_getText(String_t *str);
extern int String_getLen(String_t *str);

#ifdef __cplusplus
}
#endif

#endif

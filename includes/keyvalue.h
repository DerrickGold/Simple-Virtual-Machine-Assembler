#ifndef _KEYVALUES_H_
#define _KEYVALUES_H_



#ifdef __cplusplus
extern "C" {
#endif


/*generic object for storing a name with a value*/
typedef struct {
    char *key;
    int value;
}KeyValue_t;

extern KeyValue_t *KeyValue_create(const char *key, int value);
extern void KeyValue_destroy(KeyValue_t *kv);

extern char *KeyValue_getKey(const KeyValue_t *kv);
extern int KeyValue_getValue(const KeyValue_t *kv);


#ifdef __cplusplus
}
#endif

#endif

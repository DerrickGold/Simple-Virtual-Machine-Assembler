#ifndef _KEYVNODE_H_
#define _KEYVNODE_H_

#include <stdarg.h>
#include "btree.h"
#include "keyvalue.h"

#ifdef __cplusplus
extern "C" {
#endif


extern BTreeNode_t *KeyVNode_create(const char *key, int value);
extern void KeyVNode_destroyData(void *nodeData);
extern int KeyVNode_comparator(const BTreeNode_t *n1, const BTreeNode_t *n2);
extern KeyValue_t *KeyVNode_find(BTree_t *tree, char *key);
extern void KeyVNode_print(void *data, int (*)(const char *, ...));


#ifdef __cplusplus
}
#endif

#endif

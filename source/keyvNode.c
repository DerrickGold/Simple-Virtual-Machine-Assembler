/*=======================================================
keyvNode.c

Glues the keyvalue module with the binary tree module
for seamless interaction.
=======================================================*/
#include <string.h>
#include "debug.h"
#include "keyvNode.h"

/*=======================================================
Glue code to link modules together
=======================================================*/

/*
Creating and destorying KeyValue Nodes
*/
BTreeNode_t *KeyVNode_create (const char *key, int value) {
	msgAssert(key != NULL, "Error, Packing NULL keyvalue to node.", 1);
	return BTreeNode_create(KeyValue_create(key, value));
}

void KeyVNode_destroyData (void *data) {
	msgAssert(data != NULL, "Error: destroying NULL node");
	KeyValue_destroy((KeyValue_t *)data);
}

/*
Comparator used in the binary tree for searching and inserting
*/
int KeyVNode_comparator(const BTreeNode_t *n1, const BTreeNode_t *n2) {
	return strcmp(
			KeyValue_getKey(BTreeNode_getData(n1)),
			KeyValue_getKey(BTreeNode_getData(n2))
		);
}

/*
Retrieve a keyvalue pair from a binary tree
*/
KeyValue_t *KeyVNode_find (BTree_t *tree, char *key) {
	BTreeNode_t *tn = NULL;
	BTreeNode_t *keyNode = BTreeNode_create(KeyValue_create(key, 0));

	msgAssert(keyNode != NULL, "Error creating search key node.");

	tn = BTree_find(tree, keyNode);
	/*destroy the search node after the search has completed*/
	BTreeNode_destroy(keyNode, &KeyVNode_destroyData);

	if(!tn)
		return NULL;

	return (KeyValue_t *)BTreeNode_getData(tn);
}

void KeyVNode_print (void *data, int (*print)(const char *, ...)){
	KeyValue_t *kv = (KeyValue_t*)data;
	print("{%s->%d}", KeyValue_getKey(kv), KeyValue_getValue(kv));
}


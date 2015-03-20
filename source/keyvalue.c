/*=======================================================
keyvalue.c

A container for associating a string with an integer
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "keyvalue.h"



KeyValue_t *KeyValue_create (const char *key, int value) {
	/*first, allocate object*/
	KeyValue_t *obj = malloc(sizeof(KeyValue_t));
	msgAssert(obj != NULL, "Error allocating KeyValue_t.");

	/*then allocate name*/
	obj->key = malloc(strlen(key) + 1);
	msgAssert(obj->key != NULL, "Error allocating KeyValue_t key.");

	strcpy(obj->key, key);
	/*set mapped value*/
	obj->value = value;
	return obj;
}


void KeyValue_destroy (KeyValue_t *kv) {
	if(!kv) return;

	if(kv->key) free(kv->key);
	kv->key = NULL;
	kv->value = 0;

	free(kv);
}


char *KeyValue_getKey (const KeyValue_t *kv) {
	return kv->key;
}


int KeyValue_getValue (const KeyValue_t *kv) {
	return kv->value;
}



/*=======================================================
myString.c

A simple allocatable string module
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myString.h"
#include "debug.h"

/*
Create and initialize a string
*/
String_t *String (char *text) {
	String_t *temp = calloc(1, sizeof(String_t));
	msgAssert(temp != NULL, "Error allocating string (1): %s", text);

	temp->buf = calloc(sizeof(char), strlen(text) + 1);
	msgAssert(temp->buf != NULL, "Error allocating string text (2): %s", text);

	strcpy(temp->buf, text);
	temp->len = strlen(text);

	return temp;
}

/*
Destroy and cleanup string
*/
void String_destroy (String_t *str) {
	if(!str) return;

	if(str->buf && str->len > 0)
		free(str->buf);

	str->buf = 0;
	str->len = 0;
	free(str);
}

/*
Some self explanatory getter functions
*/

char *String_getText (String_t *str) {
	return str->buf;
}

int String_getLen (String_t *str) {
	return str->len;
}

/*=======================================================
debug.c

Some helpful functions.
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
/*
Function remains in source for ansi C support.
*/
/*
void msgAssert(char condition, const char *msg, ...) {
	va_list vl;
	if (condition) return;
	va_start(vl, msg);

	fprintf(stderr, "\nmsgAssertion:\n\t");
	vfprintf(stderr, msg, vl);
	va_end(vl);
	abort();
}*/


int dbgprintf(const char *format, ...) {
	int res = 0;

	va_list vl;
	va_start(vl, format);
	res = vfprintf(stderr, format, vl);
	va_end(vl);
	return res;
}

void printDivider (Debug_printf print) {
	int i = 0;
	for(i = 0; i < 80; i++)
		print("-");

	print("\n");
}

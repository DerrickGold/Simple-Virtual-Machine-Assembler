#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdarg.h>
#include <assert.h>

/*Using some fancy C99 feature of variadic preprocessing*/
#define msgAssert(condition, ...) 	\
	if(!(condition)) { 					\
		dbgprintf( __VA_ARGS__); 	\
		dbgprintf("\n"); 				\
		assert(condition); 				\
	}


#ifdef __cplusplus
extern "C" {
#endif


typedef int (*Debug_printf)(const char *, ...);

/*function kept back for ansi C support*/
/*extern void msgAssert(char condition, const char *msg, ...);*/

extern int dbgprintf(const char *format, ...);
extern void printDivider (Debug_printf print);

#ifdef __cplusplus
}
#endif

#endif

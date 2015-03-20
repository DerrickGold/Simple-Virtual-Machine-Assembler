#ifndef _FILEBUF_H_
#define _FILEBUF_H_

#include <stdarg.h>
#include "myString.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PrgmLine_t {
	char argCount;
	String_t *label,
		   	 *op,
		   	 *arg[2];
}PrgmLine_t;

typedef struct PrgmFile_t {
	PrgmLine_t *line;
	int lineCount;
}PrgmFile_t;


extern PrgmFile_t *PrgmFile_readFile(FILE *input);
extern void PrgmFile_closeFile(PrgmFile_t *file);
extern void PrgmFile_print(	PrgmFile_t *file,
							int (*)(const char *, ...));

extern int PrgmFile_getLineCount(PrgmFile_t *file);
extern PrgmLine_t *PrgmFile_getLine(PrgmFile_t *file, int lineNum);

extern String_t *PrgmLine_getLabel(PrgmLine_t *line);
extern String_t *PrgmLine_getOp(PrgmLine_t *line);
extern char PrgmLine_getArgCount(PrgmLine_t *line);
extern String_t *PrgmLine_getArg(PrgmLine_t *line, int argNum) ;


#ifdef __cplusplus
}
#endif

#endif

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <stdio.h>
#include "btree.h"
#include "fileBuf.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct Assembler {
	BTree_t *opCodes,
			*registers,
			*labels;

	PrgmFile_t *srcFile;

	FILE 	*output;
}Assembler_t;

extern Assembler_t *Assembler_create(FILE *input, FILE *output);
extern void Assembler_destroy(Assembler_t *assembler);
extern int Assembler_scanLabels(Assembler_t *assembler);
extern int Assembler_assemble(Assembler_t *assembler);

extern BTree_t *Assembler_getOpTree (const Assembler_t *assembler);
extern BTree_t *Assembler_getRegTree (const Assembler_t *assembler);
extern BTree_t *Assembler_getLabelTree (const Assembler_t *assembler);
extern PrgmFile_t *Assembler_getPrgm (const Assembler_t *assembler);

extern void Assembler_testEncoding (unsigned char opCode, char reg1, char reg2, short imm);

#ifdef __cplusplus
}
#endif

#endif

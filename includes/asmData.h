#ifndef _ASMDATA_H_
#define _ASMDATA_H_

#include "btree.h"
#include "keyvalue.h"


#define ENCODE_INST(op,reg1,reg2,imm) \
	(op | ((reg2 | (reg1 << 6)) << 8) | (imm << 16))

#ifdef __cplusplus
extern "C" {
#endif




extern BTree_t *AsmData_createOpcodeTree (void);
extern BTree_t *AsmData_createRegTree (void);
extern BTree_t *AsmData_createLabelTree (void);
extern unsigned char AsmData_getOpCode (KeyValue_t *kv);
extern char AsmData_getOpLength (KeyValue_t *kv);

extern unsigned int AsmData_encodeInstr(unsigned char opcode, unsigned char reg1, unsigned char reg2, unsigned short imm);
#ifdef __cplusplus
}
#endif

#endif

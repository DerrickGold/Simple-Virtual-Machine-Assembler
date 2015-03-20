#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "myString.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DBG_OUT

#define TRUE 1
#define FALSE (!TRUE)
#define COMMENT_CHAR '#'
#define PRGMLINE_TOKENSTR " ,\t"

#define DBGPRINT dbgprintf

#define LABEL_COLUMN 0
#define OPCODE_COLUMN 9

typedef enum {
	A1 = 0,
	R1 = 1,
	A2 = 2,
	R2 = 3
}REGISTERS;

typedef enum {
	/*set data op num to 0 to indicate it isn't an op*/
	DATA = 0 ,
	HALT = 0x61,
	LOAD = 0x80, LOADI,
	STORE, STOREI,
	JMP, JMPO, JMPZ, JMPN,
	ADD, ADDR,
	SUB, SUBR,
	OUTI, OUTR, OUTIC, OUTRC,
	OUT, OUTC
}OP_CODES;

/*since all opcodes are 8 bits, and are stored
in a structure holding 32 bits, utilize the 9th
bit to indicate whether the opcode uses 4 bytes.
If not, then opcode uses only 2 bytes*/
typedef enum {
	OPCODE4B = (1 << 8),
	NOTOP 	 = (1 << 9)
}OP_CODE_FLAGS;




#ifdef __cplusplus
}
#endif

#endif

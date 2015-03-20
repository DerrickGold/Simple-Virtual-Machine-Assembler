/*=======================================================
test.c

Test the various components of the assembler.
=======================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "myString.h"

#include "main.h"
#include "btree.h"
#include "keyvalue.h"
#include "keyvNode.h"
#include "assembler.h"
#include "asmData.h"
#include "fileBuf.h"

/*=======================================================
Tree related tests
=======================================================*/
static void _printTree (BTree_t *tree, const char *treeName) {
	printDivider(&DBGPRINT);
	DBGPRINT("Printing %s:\n", treeName);

	BTree_print(tree, &DBGPRINT,
				(BTree_PrintData) &KeyVNode_print);
}

static void _test_PrintTrees (Assembler_t *assembler)
{
	_printTree(Assembler_getOpTree(assembler), "Opcode Tree");
	_printTree(Assembler_getRegTree(assembler), "Register Tree");
	_printTree(Assembler_getLabelTree(assembler), "Labels Tree");
}




char _testOperation(KeyValue_t *op) {
	if(op){
		DBGPRINT("\tFound: %s->0x%x (%d byte instruction)\n", KeyValue_getKey(op),
				AsmData_getOpCode(op), AsmData_getOpLength(op));
		return TRUE;
	}

	return FALSE;
}


static void _test_OpTree (Assembler_t *assembler) {
	BTree_t *opCodes = Assembler_getOpTree(assembler);
	KeyValue_t *op = NULL;

	printDivider(&DBGPRINT);
	DBGPRINT("Op Tree Test:\n");

	/*this test must pass, or structure is broken*/
	op = KeyVNode_find(opCodes, "LOAD");
	msgAssert(_testOperation(op), "Error: no such op code \"%s\"\n", "LOAD");


	/*test an operation that doesn't exist*/
	op = KeyVNode_find(opCodes, "SOMETHING");
	msgAssert(!_testOperation(op), "Error: SOMETHING opcode exists and it shouldn't.");

	/*another test*/
	op = KeyVNode_find(opCodes, "STOREI");
	msgAssert(_testOperation(op), "Error: no such op code \"%s\"\n", "STOREI");

}

static void _test_RegTree (Assembler_t *assembler) {
	BTree_t *regs = Assembler_getRegTree(assembler);
	KeyValue_t *A1Reg = NULL;

	printDivider(&DBGPRINT);
	DBGPRINT("Register Tree Test:\n\t");

	A1Reg = KeyVNode_find(regs, "A1");
	msgAssert(A1Reg != NULL, "Error: no such register \"%s\"\n", "A1");

	DBGPRINT("Found: %s->%d\n", KeyValue_getKey(A1Reg),
				KeyValue_getValue(A1Reg));
}


/*=======================================================
My string test
=======================================================*/

static void _test_myString (void) {
	String_t *str = String("hello, world!");
	printDivider(&DBGPRINT);
	DBGPRINT("String Test:\n\t%s\n", String_getText(str));
	String_destroy(str);
}



/*=======================================================
Testing the file module
=======================================================*/
static void _test_FileBuf (Assembler_t *assembler) {
	PrgmFile_t *srcFile = Assembler_getPrgm(assembler);
	printDivider(&DBGPRINT);
	DBGPRINT("PrgmFile_t Contents:\n\n");
	PrgmFile_print(srcFile, &DBGPRINT);
}



/*=======================================================
Testing opcode encoding
=======================================================*/
static void printIntBin (unsigned int num){
	int i = 0;

	for(i = 0; i < 32; i++){
		if(i > 0 && !(i % 8))DBGPRINT("|");
		if(num & (1<<i))DBGPRINT("1");
		else DBGPRINT("0");
	}
	DBGPRINT("\n");
}

static void printBytes (unsigned int num) {
	int i = 0;
	for (i = 0; i < sizeof(unsigned int); i++) {
		unsigned char *byte = (unsigned char*)&num;
		DBGPRINT("%d|", byte[i]);
	}
	DBGPRINT("\n");
}

void _test_opEncoding (unsigned char opCode, char reg1, char reg2, short imm) {
	unsigned int instruction = 0;
	instruction = AsmData_encodeInstr(opCode, reg1, reg2, imm);

	printDivider(&DBGPRINT);
	DBGPRINT("Instruction Encoding Test:\n");
	DBGPRINT("\topCode: 0x%x\treg1: %d\treg2: %d\timm: %d\n",
			opCode, reg1, reg2, imm);
	DBGPRINT("\tint value: %d\n\tbin representation: ", instruction);
	printIntBin(instruction);
	DBGPRINT("\tbyte values:\t|");
	printBytes(instruction);
}





/*=======================================================
Everything
=======================================================*/

void Test_runAll(Assembler_t *assembler) {
	_test_myString();

	_test_PrintTrees(assembler);
	_test_OpTree(assembler);
	_test_RegTree (assembler);


	_test_FileBuf(assembler);

	_test_opEncoding(0x81, R1, A1, 113);
	_test_opEncoding(0x81, R2, A2, 16384);
	_test_opEncoding(0x81, R2, A1, 15432);
	_test_opEncoding(0x81, A2, R2, 258);
}




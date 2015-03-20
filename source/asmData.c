/*=======================================================
asmData.c

Initialize and retrieval of data required by the
assembler.

Also includes glue code for the assembler to grab
information from the KeyvNode module.
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "debug.h"
#include "myString.h"

#include "asmData.h"
#include "keyvNode.h"


/*
Create and initialize a binary tree for opcodes
*/
BTree_t *AsmData_createOpcodeTree (void) {
	BTree_t *opCodes = BTree_create(&KeyVNode_comparator, &KeyVNode_destroyData);
	msgAssert(opCodes != NULL, "Error allocating opcode tree.");

	BTree_add(opCodes, KeyVNode_create("DATA", 		DATA | NOTOP));
	BTree_add(opCodes, KeyVNode_create("HALT", 		HALT));
	BTree_add(opCodes, KeyVNode_create("LOAD", 		LOAD | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("LOADI", 	LOADI));
	BTree_add(opCodes, KeyVNode_create("STORE", 	STORE | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("STOREI", 	STOREI));
	BTree_add(opCodes, KeyVNode_create("JMP", 		JMP | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("JMPO", 		JMPO | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("JMPZ", 		JMPZ | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("JMPN", 		JMPN | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("ADD", 		ADD | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("ADDR", 		ADDR));
	BTree_add(opCodes, KeyVNode_create("SUB", 		SUB | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("SUBR", 		SUBR));
	BTree_add(opCodes, KeyVNode_create("OUTI", 		OUTI));
	BTree_add(opCodes, KeyVNode_create("OUTR", 		OUTR));
	BTree_add(opCodes, KeyVNode_create("OUTIC", 	OUTIC));
	BTree_add(opCodes, KeyVNode_create("OUTRC", 	OUTRC));
	BTree_add(opCodes, KeyVNode_create("OUT", 		OUT | OPCODE4B));
	BTree_add(opCodes, KeyVNode_create("OUTC", 		OUTC | OPCODE4B));

	return opCodes;
}

/*
Create an intitialize a binary tree for registers
*/
BTree_t *AsmData_createRegTree (void) {
	BTree_t *registers = BTree_create(&KeyVNode_comparator, &KeyVNode_destroyData);
	msgAssert(registers != NULL, "Error allocating register tree.");

	BTree_add(registers, KeyVNode_create("A1", A1));
	BTree_add(registers, KeyVNode_create("R1", R1));
	BTree_add(registers, KeyVNode_create("A2", A2));
	BTree_add(registers, KeyVNode_create("R2", R2));

	return registers;
}

/*
Create a binary tree to collect labels
*/
BTree_t *AsmData_createLabelTree (void) {
	BTree_t *labelTree = BTree_create(&KeyVNode_comparator, &KeyVNode_destroyData);
	msgAssert(labelTree != NULL, "Error Allocating label tree.");
	return labelTree;
}

/*
Get the opcode from a KeyValue_t object
*/
unsigned char AsmData_getOpCode (KeyValue_t *kv) {
	return (KeyValue_getValue(kv) & 255);
}

/*
Get size in bytes of an instruction stored in a keyvalue_t object
*/
char AsmData_getOpLength (KeyValue_t *kv) {
	int val = KeyValue_getValue(kv);
	if(val & OPCODE4B) return 4;
	return 2;
}

/*
Encode an instruction for the simple virtual machine
*/
unsigned int AsmData_encodeInstr (
	unsigned char op,
	unsigned char reg1,
	unsigned char reg2,
	unsigned short imm
)
{
  unsigned int byte2 = (reg1) | ((reg2) << 6);
  return ((unsigned int)op |  (byte2 << 8) | (imm << 16));
}

/*=======================================================
assembler.c

Assembles an input svm file and outputs its
machine code.
=======================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "debug.h"
#include "myString.h"
#include "btree.h"
#include "fileBuf.h"
#include "keyvalue.h"
#include "keyvNode.h"
#include "asmData.h"
#include "assembler.h"


/*=======================================================
Assembler start
=======================================================*/

/*some get and access functions for the Assembler_t struct*/
static void _assembler_setOpTree (Assembler_t *assembler, BTree_t *tree) {
	assembler->opCodes = tree;
}

BTree_t *Assembler_getOpTree (const Assembler_t *assembler) {
	return assembler->opCodes;
}



static void _assembler_setRegTree (Assembler_t *assembler, BTree_t *tree) {
	assembler->registers = tree;
}

BTree_t *Assembler_getRegTree (const Assembler_t *assembler) {
	return assembler->registers;
}



static void _assembler_setLabelTree (Assembler_t *assembler, BTree_t *tree) {
	assembler->labels = tree;
}

BTree_t *Assembler_getLabelTree (const Assembler_t *assembler) {
	return assembler->labels;
}



static void _assembler_setPrgm(Assembler_t *assembler, PrgmFile_t *prgm) {
	assembler->srcFile = prgm;
}

PrgmFile_t *Assembler_getPrgm (const Assembler_t *assembler) {
	return assembler->srcFile;
}



static void _assembler_setOutputDest (Assembler_t *assembler, FILE *dest) {
	assembler->output = dest;
}

static FILE *_assembler_getOutputDest (Assembler_t *assembler) {
	return assembler->output;
}


/*
Create an initialize the Assembler
*/
Assembler_t *Assembler_create (FILE *input, FILE *output) {
	Assembler_t *assembler = calloc(1, sizeof(Assembler_t));
	msgAssert(assembler != NULL, "Error allocating assembler object.");

	/*collect the output location*/
	_assembler_setOutputDest(assembler, output);

	/*generate the data trees necessary*/
	_assembler_setOpTree(assembler, AsmData_createOpcodeTree());
	_assembler_setRegTree(assembler, AsmData_createRegTree());
	_assembler_setLabelTree(assembler, AsmData_createLabelTree());

	/*read in the file*/
	_assembler_setPrgm(assembler, PrgmFile_readFile(input));

	return assembler;
}

/*
Clean up all associated memory with the assembler.
*/
void Assembler_destroy (Assembler_t *assembler) {
	if(!assembler) return;

	/*close output file handle if not output to stdout*/
	if(_assembler_getOutputDest(assembler) != stdout) {
		fclose(_assembler_getOutputDest(assembler));
		assembler->output = NULL;
	}

	/*free all memory used by trees*/
	BTree_destroy(Assembler_getOpTree(assembler));
	BTree_destroy(Assembler_getRegTree(assembler));
	BTree_destroy(Assembler_getLabelTree(assembler));

	PrgmFile_closeFile(assembler->srcFile);
	assembler->srcFile = NULL;

	free(assembler);
}


/*
Scans for labels and determines the address they point to,
and the values they contain.

Addresses are calculated by summing the length of all the
instructions prior to the label's location within the file.*/
int Assembler_scanLabels (Assembler_t *assembler) {

	PrgmFile_t 	*src 		= Assembler_getPrgm(assembler);
	BTree_t 	*labelTree 	= Assembler_getLabelTree(assembler),
				*opCodes 	= Assembler_getOpTree(assembler);

	int i = 0, addr = 0;

	for(i = 0; i < PrgmFile_getLineCount(src); i++) {
		/*current line of code to process*/
		PrgmLine_t *curLine = PrgmFile_getLine(src, i);

		/*grab the label and operation on the line*/
		String_t *label 	= PrgmLine_getLabel(curLine),
				 *opStr 	= PrgmLine_getOp(curLine);

		/*check that line contains a valid operation*/
		KeyValue_t *operation = KeyVNode_find(opCodes, String_getText(opStr));

		msgAssert(operation != NULL, "Error, no such operation \"%s\"",
				 String_getText(opStr));

		if(!label) {
			/*no label on this line, push memory address
			to next instruction and continue.*/
			addr += AsmData_getOpLength(operation);
			continue;
		}

		/*set label to point to current address*/
		BTree_add(labelTree, KeyVNode_create(String_getText(label), addr));
		/*increase address to next instruction*/
		addr += AsmData_getOpLength(operation);
	}

	return 0;
}

/*swaps a label/register string to its
integer/addr value*/
static int _labeltoi (String_t *argStr, BTree_t *tree) {
	KeyValue_t *parameter = NULL;
	char *argText = argText = String_getText(argStr);

	if((parameter = KeyVNode_find(tree, argText)))
		return KeyValue_getValue(parameter);

	return -1;
}


/*
Swaps a given argument with the correct
register, label, or immediate value.
*/
static void argtoi (
	String_t *argStr,
	BTree_t *registers,
	BTree_t	*labelTree,
	short *imm,
	unsigned char *reg
)
{
	int tempVal = 0;

	msgAssert(argStr != NULL, "Error, missing argument!");

	/*first check if argument is one of the register*/
	tempVal = _labeltoi(argStr, registers);
	if(tempVal > -1 ) {
	  *reg = (unsigned char)(tempVal);
	  return;
	}

	/*then check if argument is a label*/
	tempVal = _labeltoi(argStr, labelTree);
	if(tempVal > -1) {
		*imm = (short)tempVal;
		return;
	}

	/*lastly, if its not a register or a label,
	it's probably an immediate integer value*/
	*imm = (short)atoi(String_getText(argStr));
}

/*
Handle text replacement for data declarations.

Data doesn't always contain an immediate integer value,
sometimes it can point to another label, so a substitution
needs to be done.
*/
static short _convertData (String_t *dataStr, BTree_t *labelTree) {
	char *argText = String_getText(dataStr);
	short imm = 0;
	KeyValue_t *label = NULL;

	msgAssert(argText != NULL, "Error, missing data value");

	/*first we need to check the data isn't pointing to a label*/
	label = KeyVNode_find(labelTree, argText);
	if(label) {
		/*a label was found, substitute it*/
		imm = KeyValue_getValue(label);
	} else {
		/*otherwise, a straight value was found*/
		imm = atoi(argText);
	}

	return imm;
}

/*
Main assembler logic
*/
int Assembler_assemble (Assembler_t *assembler) {
	FILE *output 		= _assembler_getOutputDest(assembler);
	PrgmFile_t *src 	= Assembler_getPrgm(assembler);
	BTree_t *opTree 	= Assembler_getOpTree(assembler),
			*regTree 	= Assembler_getRegTree(assembler),
			*labelTree 	= Assembler_getLabelTree(assembler);

	int i = 0;
	for(i = 0; i < PrgmFile_getLineCount(src); i++) {
		/*make sure everything is set to 0 first */
		unsigned char opCode = 0, regs[2] = {0,0};
		short immValue = 0;
		int curArg = 0;
		unsigned int instruction = 0;

		/*get line and its operation and parameters*/
		PrgmLine_t *curLine = PrgmFile_getLine(src, i);

		String_t *opStr 	= PrgmLine_getOp(curLine);
		String_t *args[2];

		KeyValue_t *operation = KeyVNode_find(opTree, String_getText(opStr));

		msgAssert(operation != NULL, "Error, no such operation \"%s\"",
				String_getText(opStr));


		/*initialize the argument strings*/
		for( curArg = 0; curArg < 2; curArg++)
		  args[curArg] = PrgmLine_getArg(curLine, curArg);


		opCode = AsmData_getOpCode(operation);
		/*data op found, reserve memory*/
		if(!opCode){
			short imm = _convertData(args[0], labelTree);
			fwrite(&imm, 2, 1, output);

			/*continue on to next instruction*/
			continue;
		}

		/*replace argument text*/
		for(curArg = 0; curArg < PrgmLine_getArgCount(curLine); curArg++) {
			argtoi(args[curArg], regTree, labelTree,
					&immValue, &regs[curArg]);
		}


		/*encode instruction*/
		instruction = AsmData_encodeInstr(opCode, regs[0], regs[1], immValue);
		/*write out instruction*/
		fwrite(&instruction, AsmData_getOpLength(operation),
				1, output);
	}
	return 0;
}



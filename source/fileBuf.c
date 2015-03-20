/*=======================================================
fileBuf.c -> should really be renamed to PrgmSomething

A container for storing the lines of a simple virtual
machine assembler program.
=======================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "debug.h"
#include "fileBuf.h"

/*=======================================================
Getter and setter methods for the PrgmLine_t structure
=======================================================*/

void PrgmLine_setLabel (PrgmLine_t *line, String_t *label) {
	line->label = label;
}

String_t *PrgmLine_getLabel (PrgmLine_t *line) {
	return line->label;
}



void PrgmLine_setOp (PrgmLine_t *line, String_t *op) {
	line->op = op;
}

String_t *PrgmLine_getOp (PrgmLine_t *line) {
	return line->op;
}



void PrgmLine_setArg (PrgmLine_t *line, String_t *arg, int argNum) {
	line->arg[argNum] = arg;
}

String_t *PrgmLine_getArg (PrgmLine_t *line, int argNum) {
	return line->arg[argNum];
}



char PrgmLine_getArgCount (PrgmLine_t *line) {
	return line->argCount;
}

void PrgmLine_setArgCount (PrgmLine_t *line, char count) {
	line->argCount = count;
}



void PrgmLine_print (
	PrgmLine_t *line,
	int lineNum,
	int (*print)(const char *, ...)
)
{
	int i = 0;
	char hasLabel = 0;
	if (!line) return;

	print("%d: ", lineNum);
	if(PrgmLine_getLabel(line)){
		char *str = String_getText(PrgmLine_getLabel(line));
		print("Label: %s", str);
		hasLabel++;
	}
	if(PrgmLine_getOp(line)) {
		char *str = String_getText(PrgmLine_getOp(line));
		if(!hasLabel) print("\t\t");
		print("\tOp: %s\t", str);
	}

	for(i = 0; i < PrgmLine_getArgCount(line); i++) {
		char *str = String_getText(PrgmLine_getArg(line, i));
		print("Arg%d: %s\t", i + 1, str);
	}

	print("\n");
}



/*=======================================================
Now for the actual collection and storage of lines.
=======================================================*/

/*strip comments and newline characters from an input line*/
static void _sanitizeInputStr (char *input) {
	char *ptr = strrchr(input, COMMENT_CHAR);

	if(ptr) *ptr = '\0';
	else {
		if((ptr = strrchr(input, '\n'))) *ptr = '\0';
	}

}



/*
Add or remove lines to the file buffer as necessary.
*/
static void _resizeLineBuffer (PrgmFile_t *file, int newLineCount) {
	PrgmLine_t *tempLines = NULL;
	tempLines = realloc(file->line, sizeof(PrgmLine_t) * newLineCount);
	msgAssert(tempLines != NULL, "Error expanding line buffer(%d to %d)",
		file->lineCount, newLineCount);

	file->lineCount = newLineCount;
	file->line = tempLines;
}



/*
Stores an individual section of a line based on column position of
the input text.

By strings delimited by spaces:
Column 0 - program label,
Column 1 - program operation
Column 2 - argument 1
Column 3 - argument 2
*/
static void _storeSection (PrgmLine_t *curLine, char *tok, const char *startPos, char *wordColumn) {
	int columnStart = (int)(tok - startPos);

	switch(*wordColumn) {
		case 0:
			if(columnStart == LABEL_COLUMN) {
				PrgmLine_setLabel(curLine, String(tok));
				break;
			}
			else (*wordColumn)++;
		case 1:
			PrgmLine_setOp(curLine, String(tok));
		break;
		default: {
			char count = PrgmLine_getArgCount(curLine);
			PrgmLine_setArg(curLine, String(tok), count++);
			PrgmLine_setArgCount(curLine, count);
		}break;

	}
	(*wordColumn)++;
}


/*
Read in an input file to the PrgmFile_t struct.

This function uses block allocations in an attempt
to reduce the number of allocation calls needed to
store the entire file.
*/
PrgmFile_t *PrgmFile_readFile (FILE *input) {
	int curLine = 0;
	char lineBuf[256];

	/*first allocate structure memory*/
	PrgmFile_t *file = calloc(1, sizeof(PrgmFile_t));
	msgAssert(file != NULL, "Error allocating PrgmFile_t.");

	/*allocate some memory for lines. Start off with 20,
	resize when limit is hit later on.*/
	_resizeLineBuffer(file, 20);

	/*read in the input file now*/
	while(!feof(input)) {
		char wordColumn = 0;
		char *cptr = NULL;
		char *tempLine = fgets(lineBuf, sizeof(lineBuf), input);

		if (!tempLine) break;

		/*resize line buffer if we hit the end*/
		if(curLine >= file->lineCount)
			_resizeLineBuffer(file, file->lineCount << 1);

		_sanitizeInputStr(tempLine);
		/*make sure line isn't empty*/
		if(!strlen(tempLine)) continue;

		/*break up and store the lines*/
		cptr = strtok(tempLine, PRGMLINE_TOKENSTR);
		msgAssert(cptr != NULL, "Failed to tokenize line:\n%s@line: %d", tempLine, curLine);
		while(cptr){
			_storeSection(&file->line[curLine], cptr, (const char*)tempLine, &wordColumn);
			cptr = strtok(NULL, PRGMLINE_TOKENSTR);
		}
		curLine++;
	}

	/*now that we have the actual number of lines counted, we can
	shrink the file buffer to only use the space it needs*/
	if(curLine < file->lineCount)
		_resizeLineBuffer(file, curLine);

	return file;
}

void PrgmFile_closeFile(PrgmFile_t *file) {
	if(PrgmFile_getLineCount(file) > 0){

		int i = 0;
		for(i = 0; i < PrgmFile_getLineCount(file); i++){
			int argc = 0;
			PrgmLine_t *curLine = PrgmFile_getLine(file, i);
			//clear everything in the line
			String_destroy(curLine->label);
			curLine->label = NULL;
			String_destroy(curLine->op);
			curLine->op = NULL;

			for(argc = 0; argc < curLine->argCount; argc++){
				String_destroy(curLine->arg[argc]);
				curLine->arg[argc] = NULL;
			}
		}
		//free all the lines
		free(file->line);
		file->line = NULL;
	}
	free(file);
}

/*
Get a line by line number from the stored program
*/
PrgmLine_t *PrgmFile_getLine (PrgmFile_t *file, int lineNum) {
	msgAssert(lineNum >= 0 && lineNum < file->lineCount,
			"Error: PrgmLine_t number out of range: %d", lineNum);

	return &file->line[lineNum];
}

int PrgmFile_getLineCount (PrgmFile_t *file) {
	return file->lineCount;
}


void PrgmFile_print (
	PrgmFile_t *file,
	int (*print)(const char *, ...)
)
{
	int i = 0;
	for(i = 0; i < PrgmFile_getLineCount(file); i++) {
		PrgmLine_t *curLine = PrgmFile_getLine(file, i);
		if(curLine) PrgmLine_print(curLine, i, print);
	}
}

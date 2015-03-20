/*********************************************************
#Student's Name: Derrick Gold
#Assignment #2
#CMPT 201, Class Section: AS40(1)
*********************************************************/
/********************************************************
Description:


********************************************************/


#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "assembler.h"
#include "tests.h"


/*=======================================================
Main
=======================================================*/

int main (int argc, char *argv[]) {

	Assembler_t *assembler = Assembler_create(stdin, stdout);


	Assembler_scanLabels(assembler);

	#ifdef DBG_OUT
		Test_runAll(assembler);
	#endif

	Assembler_assemble(assembler);
	Assembler_destroy(assembler);
	return 0;
}


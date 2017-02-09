/*
	strsrch
	Small tool for scanning a binary blob/file for possible text strings
	damian@sudden-desu.net
*/
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// minimum length of characters to be considered a string
unsigned long minLength = 5;
// size of found string buffer
long strBufSize = 4096;
FILE *inFile = NULL;

int main(int argc, char **argv) {
	int argResult = processArgs(argc, argv);
	if (argResult != 0) return argResult;

	if (inFile == NULL) {
		inFile = stdin;
	}

	int inChar;
	unsigned long inPtr = 0,     // input file read offset
			thisStringPos = 0,        // array offset for the found string in this iteration
			string_start_offset = 0;  // offset of the start of the found string inside file

		char thisString[strBufSize];

	while (!feof(inFile)) {
		inChar = getc(inFile);
		if (isAscii(inChar)) {
			thisString[thisStringPos++] = inChar;
			if (!string_start_offset) string_start_offset = inPtr;
			// make sure we don't overflow the string buffer
			if(thisStringPos >= sizeof(thisString))
				goto print_found_string;
		} else {
			print_found_string:
			if (thisStringPos >= minLength) {
				thisString[thisStringPos] = '\0';
				printf("%X %s\n", string_start_offset, thisString);
				string_start_offset = 0;
				thisStringPos = 0;
			} else {
				string_start_offset = 0;
				thisStringPos = 0;
			}
		}
		inPtr++;
	}

	fclose(inFile);
	return 0;
}

int isAscii(int checkChar) { return (checkChar >= 0x20 & checkChar <= 0x7e); }

int processArgs(int argc, char **argv) {
	if (argc < 2) return 0;

	for (int argLoop = 1; argLoop < argc; argLoop++) {
		// =================== -l - set minimum length of string
		if (!strcmp(argv[argLoop], "-l")) {
			if (++argLoop >= argc) {
				printf("Nothing specified for -l option\n");
				return 1;
			}
			minLength = strtol(argv[argLoop], NULL, 10);
			if (minLength < 1) {
				printf("Invalid value for minimum length\n");
				return 1;
			}
		}

		// =================== -b - string buffer size
		else if (!strcmp(argv[argLoop], "-b")) {
			if (++argLoop >= argc) {
				printf("Nothing specified for -b option\n");
				return 3;
			}
			strBufSize = strtol(argv[argLoop], NULL, 10);
			if (strBufSize < 1) {
				printf("Invalid value for string buffer size\n");
				return 3;
			}
		}

		// =================== filename
		else {
			inFile = fopen(argv[argLoop], "r");
			if (inFile == NULL) {
				printf("File could not be opened\n");
				return 2;
			}
		}
	}

	return 0;
}
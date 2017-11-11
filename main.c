/*
	stringsearch
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
// search using Shift-JIS encoding
int sjisMode = 0;
int littleEndian = 0;

FILE *inFile = NULL;

	unsigned long inPtr = 0,     // input file read offset
		thisStringPos = 0,        // array offset for the found string in this iteration
		stringAddr = 0;  // offset of the start of the found string inside file


int main(int argc, char **argv) {

	int argResult = processArgs(argc, argv);
	if (argResult != 0) return argResult;

	if (sjisMode) {
		strBufSize = strBufSize * 2;
		minLength = minLength * 2;
	}

	if (inFile == NULL) {
		inFile = stdin;
	}

	
	if (sjisMode) {
		doSjisSearch(inFile);
	} else {
		doAsciiSearch(inFile);
	}
		
	fclose(inFile);
	return 0;
}

int doSjisSearch(FILE *inFile) {
	char byte1, byte2;
	char most, least;

	char thisString[strBufSize];

	while (!feof(inFile)) {
		// get first byte, see if it is ascii
		byte1 = getc(inFile);
		
		if(isAscii(byte1)) {
			//printf("found ascii\n");
			thisString[thisStringPos++] = byte1;
			inPtr++;
			goto foundChar;
		}
		
		// not ascii, move on to double byte sjis
		byte2 = getc(inFile);
	
		// set endian-ness...
		if (littleEndian) {
			most = byte2; least = byte1;
		}	else {
			most = byte1; least = byte2;
		}

		if (isSjis(most, least)) {
			printf("found sjis\n");
			thisString[thisStringPos++] = most;
			thisString[thisStringPos++] = least;
			inPtr += 2;
			goto foundChar;
		}

		// invalid value
		// check if we're hitting this after a qualifying string
		// (is it long enough?)
		if(thisStringPos >= minLength) {
			printFoundString:
			thisString[thisStringPos] = '\0';
			fwrite(&thisString, sizeof(thisString), 1, stdout);
		}
		stringAddr = 0;
		thisStringPos = 0;
		continue;

		foundChar:
		// if the offset has been reset, get the new address
		if(!stringAddr) stringAddr = inPtr;
		// check if number of found chars so far overflows array
		if(thisStringPos >= sizeof(thisString))
			goto printFoundString;
	}

	return 0;
}

int doAsciiSearch(FILE *inFile) {
	int inChar;
char thisString[strBufSize];

	while (!feof(inFile)) {
		inChar = getc(inFile);
		if (isAscii(inChar)) {
			thisString[thisStringPos++] = inChar;
			if (!stringAddr) stringAddr = inPtr;
			// make sure we don't overflow the string buffer
			if(thisStringPos >= sizeof(thisString))
				goto print_found_string;
		} else {
			print_found_string:
			if (thisStringPos >= minLength) {
				thisString[thisStringPos] = '\0';
				printf("%X %s\n", stringAddr, thisString);
				stringAddr = 0;
				thisStringPos = 0;
			} else {
				stringAddr = 0;
				thisStringPos = 0;
			}
		}
		inPtr++;
	}
	return 0;
}

int isAscii(int checkChar) { return (checkChar >= 0x20 & checkChar <= 0x7e); }

int combBytes(char most, char least) {
	return least | (most << 8);
}

int isSjis(char most, char least) {
	// this isn't going to be the most exact way to find sjis
	// just going to compare value to known sjis ranges
	// http://www.rikai.com/library/kanjitables/kanji_codes.sjis.shtml
	// sjis ranges (byte 1 = most, byte 2 = least):
	// byte 1 - standard printable ascii range (use above), ignore byte2
	// (81 <= byte 1 <= 9f || E0 <= byte1 <= ef) && (40 <= byte2 <= 7e || 80 <= byte2 <= FC)
	// a1 <= byte 1 <= df, ignore second byte (half width katakana)
	// 
/*
	return
		(isAscii(most) || 
		((most >= 0x81 | most <= 0x9f | most >= 0xe0 | most <= 0xef) && (least >= 0x40 | least <= 0x7e | least >= 0x80 | least <= 0xfc)) |
		most >= 0xa1 | most <= 0xdf);
		*/
	return (isAscii(most));
}

int processArgs(int argc, char **argv) {
	if (argc < 2) return 0;

	for (int argLoop = 1; argLoop < argc; argLoop++) {
		// =================== -s - search for double-byte Shift-JIS encoding
		if (!strcmp(argv[argLoop], "-s")) {
			sjisMode = 1;

		}

		// =================== -e - change to little endian mode (for sjis)
		else if (!strcmp(argv[argLoop], "-e")) {
			littleEndian = 1;
		}

		// =================== -l - set minimum length of string
		else if (!strcmp(argv[argLoop], "-l")) {
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

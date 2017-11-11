#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
int doAsciiSearch(FILE *inFile);
int doSjisSearch(FILE *inFile);
int isAscii(int checkChar);
int processArgs(int argc, char **argv);
int combBytes(char most, char least);
int isSjis(char most, char least);

#endif

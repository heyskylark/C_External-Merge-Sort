/*
 * function.h
 *
 *  Created on: May 6, 2016
 *      Author: Brandon Feist
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_
#include <stdio.h>
#include <stdlib.h>

#define MEMORYSIZE 1000  // memory size = 1000 bytes
#define WORDSIZE 256	// size to hold single words

int createSortFile(FILE *fp);	// Creates a combined file called toSort.txt from a given file list.
void filecopy(FILE *fp, FILE *ofp, char *fileName);	// Tokens file content from fp and outputs them to ofp.
void separationSort(FILE *toSort); // Takes a filepath and sorts it into separate 1000 byte files using bubble sort.
void swap(char a[], char b[]);	// Swaps two given char arrays.
void bubble_sort(char token[][WORDSIZE], char tokenPos[][WORDSIZE], char fileID[][WORDSIZE], int size);	// Takes a list of string and sorts them by token, then tokenPos, then fileID.
void outputToFile(char token[][WORDSIZE], char tokenPos[][WORDSIZE], char fileID[][WORDSIZE], int size, FILE *of);	// Output token info to given file;
void exMergeSort();	// Does external merge sort algorithm
char* getToken(FILE *ifp);	//Returns a token, token position, and fileID of the token.
char* stringConcat(char *s1, char *s2);	//Concats two strings together.

#endif /* FUNCTION_H_ */

/*
 * function.c
 *
 *  Created on: May 6, 2016
 *      Author: Brandon Feist
 */

#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int fileNum = 1;

/**
 * Creates a file that combines multiple files in a given file list.
 * This file also stores the fileId and token position of each token in the combined files.
 * If 0 is returned, the file merge was a success. If 1 is returned the file merge did not work.
 */
int createSortFile(FILE *fp) {
	FILE *toSort, *subFile;
	int pathChar, filepathCount = 0;
	char filepath[500] = {0};

	//Open new toSort.txt file.
	if ((toSort = fopen("toSort.txt", "w+")) == NULL) {
		printf("can't create or open toSort.txt\n");
		return 1;
	}

	//Iterate through filelist inputing file content into toSort.txt
	while ((pathChar = getc(fp)) != EOF) {
		//Iterate filepath string until newline.
		if(pathChar != 13 && pathChar != 10) {
			filepath[filepathCount] = pathChar;
			filepathCount++;
		} else {
			char *filepathPointer = filepath;	//Make char pointer for whole char string filepath.

			//Open filepath and input alphabet, numbers, and space tokens into toSort.txt using filecopy()
			if ((subFile = fopen(filepathPointer, "r")) == NULL) {
				printf("can't open %s\n", filepathPointer);
				return 1;
			}
			filecopy(subFile, toSort, filepathPointer);
			fclose(subFile);

			filepathCount = 0;						//Reset the filepathCounter for next filepath in file_list.txt
			memset(filepath, 0, sizeof filepath);	//Reset filepath char array for next filepath in file_path.txt
		}
	}

	//Get last filepath that is missed in the loop
	char *filepathPointer = filepath;

	//Open filepath and input alphabet, numbers, and space tokens into toSort.txt using filecopy()
	if ((subFile = fopen(filepathPointer, "r")) == NULL) {
		printf("can't open %s\n", filepathPointer);
		return 1;
	}
	filecopy(subFile, toSort, filepathPointer);
	fclose(subFile);
	fclose(toSort);

	return 0;
}

/**
 * filecopy takes a given output and input file as well the the string of the output filename.
 * filecopy then tokens the output file into words, numbers, and spaces into the input file.
 * It also includes the filename the token came from and the position the token was in the output file.
 */
void filecopy(FILE *fp, FILE *ofp, char *filename) {
	int c, tokenPos = 0;

	//Continue looping through the file until end of file is reached.
	while ((c = getc(fp)) != EOF) {
		//If c is an upper or lower case alphabet char
		if((64 < c && c < 91) || (96 < c && c < 123)) {
			//Loop until alphabetic char is not seen, indicating end of token.
			do {
				fprintf(ofp,"%c", c);
				c = getc(fp);
			} while (((64 < c && c < 91) || (96 < c && c < 123)) && (c != EOF));
			fprintf(ofp, "\t%d\t%s\n", tokenPos, filename);
			ungetc(c, fp);
			tokenPos++;	//Increment tokenPos for next token.

			//If c is a number char
		} else if((47 < c && c < 58)) {
			//Loop until a number char is not seen, indicating end of token.
			do {
				fprintf(ofp,"%c", c);
				c = getc(fp);
			} while ((47 < c && c < 58) && (c != EOF));
			fprintf(ofp, "\t%d\t%s\n", tokenPos, filename);
			ungetc(c, fp);
			tokenPos++;	//Increment tokenPos for next token.

			//If c is any char besides ctrl char
		} else if((31 < c && c < 48) || (57 < c && c < 65) || (90 < c && c < 97) || (122 < c && c < 127)) {
			fprintf(ofp,"%c\t%d\t%s\n", c, tokenPos, filename);
			tokenPos++;
		}
	}
}


/**
 * Goes through a given file and separates that file into sorted 1000 byte files using bubble sort.
 */
void separationSort(FILE *toSort) {
	FILE *fp;
	char token[WORDSIZE] = {0};
	char tokenMem[MEMORYSIZE][WORDSIZE] = {{0}}; char tokenPosMem[MEMORYSIZE][WORDSIZE] = {{0}}; char fileIdMem[MEMORYSIZE][WORDSIZE] = {{0}};
	int c, i, bytes = 0, mode = 0;
	int stringIndex = 0, memIndex = 0;

	while ((c = getc(toSort)) != EOF) {
		//Retrieving token
		if(mode == 0) {
			//Retrieve token until tab char (9) is found.
			if(c != 9) {
				token[stringIndex] = c;
				stringIndex++;

				//Check if room in memory to store token (<= 1000 bytes)
			} else {
				if((bytes + strlen(token)) <= MEMORYSIZE) {
					//put token in tokenMem
					for(i = 0; i < sizeof(token); i++) {
						tokenMem[memIndex][i] = token[i];
					}
					stringIndex = 0;	//Reset stringIndex for tokenPos

					bytes += strlen(token);	//Add token size to bytes

					memset(token, 0, sizeof(token));	//Reset token array for next token

					mode = 1;	//Switch to tokenPos mode
				} else {
					bubble_sort(tokenMem, tokenPosMem, fileIdMem, memIndex);	//Sort tokens

					//Output sorted to file
					char fileName[33];
					sprintf(fileName,"%d.txt", fileNum);
					//strcpy(files[1], fileName);
					if ((fp = fopen(fileName, "w+")) == NULL) {
						printf("%s\n", strerror(errno));
						printf("Output sorted file: can't create or open %s\n", fileName);
					}
					outputToFile(tokenMem, tokenPosMem, fileIdMem, memIndex, fp);
					fileNum++;

					//Reset all mem arrays, memIndex, bytes
					memset(tokenMem, 0, sizeof(tokenMem[0][0]) * MEMORYSIZE * WORDSIZE);
					memset(tokenPosMem, 0, sizeof(tokenPosMem[0][0]) * MEMORYSIZE * WORDSIZE);
					memset(fileIdMem, 0, sizeof(fileIdMem[0][0]) * MEMORYSIZE * WORDSIZE);
					memIndex = 0; bytes = 0;

					//add token to reset tokenMem and increase bytes
					for(i = 0; i < sizeof(token); i++) {
						tokenMem[memIndex][i] = token[i];
					}
					stringIndex = 0;	//Reset stringIndex for tokenPos

					bytes += strlen(token);	//Add token size to bytes

					memset(token, 0, sizeof(token));	//Reset token array for next token

					mode = 1;	//Switch to tokenPos mode
				}
			}

			//Retrieving tokenPos
		} else if(mode == 1) {
			//Retrieve tokenPos until tab char (9) is found.
			if(c != 9) {
				tokenPosMem[memIndex][stringIndex] = c;
				stringIndex++;
			} else {
				stringIndex = 0; //Reset stringIndex for fileID
				mode = 2;	//Switch to fileID mode
			}

			//Retrieving fileID
		} else {
			//Retrieve fileID until newline char (10) is found.
			if(c != 10) {
				fileIdMem[memIndex][stringIndex] = c;
				stringIndex++;
			} else {
				stringIndex = 0; //Reset stringIndex for token.
				memIndex++;	//Increase array counter
				mode = 0;	//Switch to token mode
			}

		}
	}
	//Sort the last and final file.
	bubble_sort(tokenMem, tokenPosMem, fileIdMem, memIndex);	//Sort tokens

	//Output sorted to file
	char fileName[33];
	sprintf(fileName,"%d.txt", fileNum);
	//strcpy(files[fileNum - 1], fileName);
	if ((fp = fopen(fileName, "w+")) == NULL) {
		printf("%s\n", strerror(errno));
		printf("Output sorted file: can't create or open %s\n", fileName);
	}
	outputToFile(tokenMem, tokenPosMem, fileIdMem, memIndex, fp);
	fileNum++;
}

/*
 * Swaps two given char arrays.
 */
void swap(char a[], char b[]) {
	char t[WORDSIZE] = { 0 };
	strcpy(t, a);
	strcpy(a, b);
	strcpy(b, t);
}

/**
 * Sorts a list of strings. Sorts according to token, if token is the same
 * it sorts by tokenPos, and if that is the same it sorts by fileID.
 */
void bubble_sort(char token[][WORDSIZE], char tokenPos[][WORDSIZE], char fileID[][WORDSIZE], int size) {
	int i, j;

	for (i = 0 ; i < ( size - 1 ); i++) {
		for (j = 0 ; j < size - i - 1; j++) {
			if(strcmp(token[j], token[j + 1]) != 0) {
				//Compare tokens
				if (strcmp(token[j], token[j + 1]) > 0) {
					swap(token[j], token[j + 1]);
					swap(tokenPos[j], tokenPos[j + 1]);
					swap(fileID[j], fileID[j + 1]);
				}
			} else if(strtol(tokenPos[j], NULL, 10) != strtol(tokenPos[j + 1], NULL, 10)) {
				//Compare tokenPos
				if(strtol(tokenPos[j], NULL, 10) > strtol(tokenPos[j + 1], NULL, 10)) {
					swap(token[j], token[j + 1]);
					swap(tokenPos[j], tokenPos[j + 1]);
					swap(fileID[j], fileID[j + 1]);
				}
			} else {
				//Compare tokenID
				if(strcmp(fileID[j], fileID[j + 1]) > 0) {
					swap(token[j], token[j + 1]);
					swap(tokenPos[j], tokenPos[j + 1]);
					swap(fileID[j], fileID[j + 1]);
				}
			}
		}
	}
}

/**
 * Outputs given token, tokenPos, and fileID array to given file (of)
 */
void outputToFile(char token[][WORDSIZE], char tokenPos[][WORDSIZE], char fileID[][WORDSIZE], int size, FILE *of) {
	int bytes = 0;
	int i;
	for(i = 0; i < size; i++) {
		bytes += strlen(token[i]);
		fprintf(of, "%s\t%s\t%s\n",token[i],tokenPos[i],fileID[i]);
	}
	fclose(of);
}

/**
 * External merge sort takes the files array and merge sorts the files two at a time into new files.
 * The exMergeSort will process the files until all the data has been sorted by token, tokenPos, then fileID
 * into a single file.
 * The exMergeSort will delete old fragment files that are not necessary for the sort anymore after being merged.
 */
void exMergeSort() {
	int i, tokenIndex = 0, tokenInfoIndex = 0, mode = 0, fileNum2 = 1;
	FILE *f1, *f2, *f3;
	char *tokenInfo1, *tokenInfo2;
	char token1[WORDSIZE] = {0}; char tokenPos1[WORDSIZE] = {0}; char fileID1[WORDSIZE] = {0};
	char token2[WORDSIZE] = {0}; char tokenPos2[WORDSIZE] = {0}; char fileID2[WORDSIZE] = {0};

	printf("external merge sort");

	for(i = 0; i < fileNum - 1; i += 2) {

		// Open new file to merge into
		char fileName[33]; char fileName1[33], fileName2[33];
		sprintf(fileName,"%d.txt", fileNum);
		//strcpy(files[fileNum - 1], fileName);
		if ((f3 = fopen(fileName, "w+")) == NULL) {
			printf("%s\n", strerror(errno));
			printf("Merge sort file f3: can't create or open %s\n", fileName);
		}


		// Rewind first and second files that will be merged.
		sprintf(fileName1,"%d.txt", fileNum2);
		if ((f1 = fopen(fileName1, "r")) == NULL) {
			printf("%s\n", strerror(errno));
			printf("Merge Sort f1: can't create or open %s\n", fileName);
		}
		fileNum2++;
		sprintf(fileName2,"%d.txt", fileNum2);
		if ((f2 = fopen(fileName2, "r")) == NULL) {
			printf("%s\n", strerror(errno));
			printf("Merge Sort f2: can't create or open %s\n", fileName);
		}
		fileNum2++;
		rewind(f1); rewind(f2);

		//Get and compare tokens until f1 or f2 runs out of tokens.
		tokenInfo1 = getToken(f1); tokenInfo2 = getToken(f2);
		while(tokenInfo1 && tokenInfo2) {
			//Break tokenInfo1 into token, tokenPos, and fileID to be compared.
			memset(token1, 0, sizeof(token1));
			memset(tokenPos1, 0, sizeof(tokenPos1));
			memset(fileID1, 0, sizeof(fileID1));
			while(mode < 3) {
				if(tokenInfo1[tokenInfoIndex] == 9 || tokenInfo1[tokenInfoIndex] == 10) {
					mode++;
					tokenIndex = 0;
				} else if(mode == 0) {
					token1[tokenIndex] = tokenInfo1[tokenInfoIndex];
					tokenIndex++;
				} else if(mode == 1) {
					tokenPos1[tokenIndex] = tokenInfo1[tokenInfoIndex];
					tokenIndex++;
				} else {
					fileID1[tokenIndex] = tokenInfo1[tokenInfoIndex];
					tokenIndex++;
				}
				tokenInfoIndex++;
			}
			mode = 0, tokenIndex = 0, tokenInfoIndex = 0;

			//Break tokenInfo2 into token, tokenPos, and fileID to be compared.
			memset(token2, 0, sizeof(token2));
			memset(tokenPos2, 0, sizeof(tokenPos2));
			memset(fileID2, 0, sizeof(fileID2));
			while(mode < 3) {
				if(tokenInfo2[tokenInfoIndex] == 9 || tokenInfo2[tokenInfoIndex] == 10) {
					mode++;
					tokenIndex = 0;
				} else if(mode == 0) {
					token2[tokenIndex] = tokenInfo2[tokenInfoIndex];
					tokenIndex++;
				} else if(mode == 1) {
					tokenPos2[tokenIndex] = tokenInfo2[tokenInfoIndex];
					tokenIndex++;
				} else {
					fileID2[tokenIndex] = tokenInfo2[tokenInfoIndex];
					tokenIndex++;
				}
				tokenInfoIndex++;
			}
			mode = 0, tokenIndex = 0, tokenInfoIndex = 0;

			//Compare tokens, lesser token gets put into new file.
			//Get another token to replace token that was put into the new file
			if(strcmp(token1, token2) != 0) {
				if(strcmp(token1, token2) < 0) {
					fprintf(f3,"%s", tokenInfo1); //Print tokenInfo1 to new file
					free(tokenInfo1);
					tokenInfo1 = getToken(f1); //Get new tokenInfo1

				} else {
					fprintf(f3,"%s", tokenInfo2); //Print tokenInfo2 to new file
					free(tokenInfo2);
					tokenInfo2 = getToken(f2);//Get new tokenInfo2
				}
			} else if(strtol(tokenPos1, NULL, 10) != strtol(tokenPos2, NULL, 10)) {
				if(strtol(tokenPos1, NULL, 10) < strtol(tokenPos2, NULL, 10)) {
					fprintf(f3,"%s", tokenInfo1); //Print tokenInfo1 to new file
					free(tokenInfo1);
					tokenInfo1 = getToken(f1); //Get new tokenInfo1
				} else {
					fprintf(f3,"%s", tokenInfo2); //Print tokenInfo2 to new file
					free(tokenInfo2);
					tokenInfo2 = getToken(f2);	//Get new tokenInfo2
				}
			} else {
				if(strcmp(fileID1, fileID2) < 0) {
					fprintf(f3,"%s", tokenInfo1); //Print tokenInfo1 to new file
					free(tokenInfo1);
					tokenInfo1 = getToken(f1); //Get new tokenInfo1
				} else {
					fprintf(f3,"%s", tokenInfo2); //Print tokenInfo2 to new file
					free(tokenInfo2);
					tokenInfo2 = getToken(f2);	//Get new tokenInfo2
				}
			}
		}

		//If there are more tokens in f1, put the rest into the new file.
		while(tokenInfo1) {
			fprintf(f3, "%s", tokenInfo1);
			free(tokenInfo1);
			tokenInfo1 = getToken(f1);
		}

		//If there are more tokens in f2, put the rest into the new file
		while(tokenInfo2) {
			fprintf(f3, "%s", tokenInfo2);
			free(tokenInfo2);
			tokenInfo2 = getToken(f2);
		}

		fclose(f1); fclose(f2); fclose(f3);
		//Delete fragmant files that aren't necessary to the sort anymore
		remove(fileName1);
		if(i < fileNum - 2) {
			remove(fileName2);
		}
		fileNum++;	// Increment fileNum
	}
}

/**
 * Returns a copy of the token and token info such as tokenPos and fileID.
 */
char* getToken(FILE *ifp) {
	int c, i = 0;
	char token[WORDSIZE * 3] = {0};
	char *ret;

	while((c = getc(ifp)) != EOF) {
		if(c != 10) {
			token[i] = c;
		} else {
			token[i] = c;
			ret = malloc(1 + strlen(token));
			strcpy(ret, token);
			return ret;
		}
		i++;
	}
	return 0;
}

/**
 * Concats two given strings into a new string.
 */
char* stringConcat(char *s1, char *s2) {
	char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

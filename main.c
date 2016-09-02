/*
 * main.c
 *
 *  Created on: May 6, 2016
 *      Author: Brandon Feist
 */

#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Sort alphabet tokens, numbers tokens, and space byte tokens in a given file list.

int main() {
	FILE *fp;
	char filename[100];

	printf("Please enter the location of the file list: ");
	scanf("%s",filename);

	//First merge all the files in the file list into one single big file.
	if ((fp = fopen(filename, "r")) == NULL) {
		printf("can't open %s\n",filename);
		return 1;
	}
	createSortFile(fp);
	fclose(fp);

	//Sort the file out into subfiles in 1000 byte chunks using bubble sort
	if ((fp = fopen("toSort.txt", "r")) == NULL) {
		printf("can't open file_list.txt\n");
		return 1;
	}
	separationSort(fp);
	fclose(fp);

	//Sort the new bubble sorted files using merge sort.
	exMergeSort();

	printf("success");

	return 0;
}

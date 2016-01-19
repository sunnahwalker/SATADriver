#include <iostream>
#include <stdio.h>
#include <Windows.h>

//	This function sorts out the print buffer for IDFY
//
//	In: Array to the Buffer that needs to be sorted out for prints
//	Out: void
//
void sorted_print(UCHAR input[], int size) {

	int i;
	for (i = 0; i < size; i += 2) {
		printf("%c%c", input[i + 1], input[i]);
	}

}
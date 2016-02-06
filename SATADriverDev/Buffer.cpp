#include <iostream>
#include <stdio.h>
#include <Windows.h>

int get_value(void *buffer, int size) {
	
	int i, sum = 0;
	UINT8 * pointer = (UINT8*)buffer;

	for (i = 0; i < size; i++) {
		sum += pointer[i] << (i * 8);
	}

	return sum;
}
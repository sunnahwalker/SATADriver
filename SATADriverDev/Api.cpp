#include <iostream>
#include <stdio.h>
#include <Windows.h>

//Local Pre Processors
//#define DEBUG_PRINTS_2
#define MAX_DEVICE 32

HANDLE Sata_Drive;

//	This API probes in from 0 to MAX_DEVICE physical slots and checks for drives
//
//	In:	Nothing
//	Out: If a valid drive is found, it will return TRUE, or else FALSE
//
BOOL find_port() {

	TCHAR port[64];
	LPTSTR line = L"\\\\.\\PhysicalDrive";
	LPCTSTR format = L"%s%d";

	for (int i = 0; i < MAX_DEVICE; i++) {
		wsprintf(port, format, line, i);

		Sata_Drive = CreateFile(port,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		if (Sata_Drive == INVALID_HANDLE_VALUE) {
			if (GetLastError() == ERROR_ACCESS_DENIED) {
				printf("Access is denied. Please run in Admin mode!!!...\n\n");;
			}
#if defined DEBUG_PRINTS_2
			else {
				printf("Failed to get handle! Port: %d Error: %d\n", i, GetLastError());
			}
#endif
		}
		else {
			printf("SATA drive found in port: %d\n", i);
			return TRUE;
			//break;
		}

	}

	//If we reach here that means we couldn't find any valid drives!
	return FALSE;
}


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
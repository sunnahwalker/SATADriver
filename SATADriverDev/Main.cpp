//////////////////////////////////////////////////////
//
//		App to check identify device and SMART data  
//		Author: Ahsan Uddin
//		© Sunnahwalker Productions
//		Version: 1.1 Release: 1/17/2015
//
//////////////////////////////////////////////////////

//Global includes
#include <iostream>
#include <stdio.h>
#include <Windows.h>

//Local includes
#include "Api.h"

//Windows DDK specific includes
#include <ntddscsi.h>
#include <Ata.h>

//Local Defines
#define VERSION 01
#define MAX_DEVICE 32
//#define DEBUG_PRINTS


int main() {

	printf("Welcome to Ahsan's SATA drive info program, v%02d\n", VERSION);
	printf("Code compiled on: %s %s\n\n", __DATE__, __TIME__);

	TCHAR port[64];
	LPTSTR line = L"\\\\.\\PhysicalDrive";
	LPCTSTR format = L"%s%d";

	HANDLE Sata_Drive;

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
#if defined DEBUG_PRINTS
			else {
				printf("Failed to get handle! Port: %d Error: %d\n", i, GetLastError());
			}
#endif
		}
		else {
			printf("SATA drive found in port: %d\n", i);
			break;
		}

	}

	if (Sata_Drive == INVALID_HANDLE_VALUE) {
		printf("No valid drives found, program will exit :(\n\n");
		exit(EXIT_FAILURE);
	}

	void *Buffer = malloc(sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA));
	memset(Buffer, 0, sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA));

	ATA_PASS_THROUGH_EX *ATA_Buffer =  (ATA_PASS_THROUGH_EX *) Buffer;

	ATA_Buffer->AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
	ATA_Buffer->Length = sizeof(ATA_PASS_THROUGH_EX);
	ATA_Buffer->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);
	ATA_Buffer->DataTransferLength = sizeof(IDENTIFY_DEVICE_DATA);
	ATA_Buffer->TimeOutValue = 10;
	ATA_Buffer->CurrentTaskFile[6] = ID_CMD;

	BOOL ret = DeviceIoControl(	Sata_Drive,
								IOCTL_ATA_PASS_THROUGH,
								Buffer, (sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA)),
								Buffer, (sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA)),
								NULL,
								NULL
								);

	CloseHandle(Sata_Drive);

	IDENTIFY_DEVICE_DATA *IDFY_Buffer;

	IDFY_Buffer = (IDENTIFY_DEVICE_DATA *)(int(Buffer) + sizeof(ATA_PASS_THROUGH_EX));

	printf("Model Number is: ");
	sorted_print((IDFY_Buffer->ModelNumber), 40);
	printf("\n");

	printf("Serial Number is: ");
	sorted_print((IDFY_Buffer->SerialNumber), 20);
	printf("\n");

	printf("FW Rev is: ");
	sorted_print((IDFY_Buffer->FirmwareRevision), 8);
	printf("\n");
	
	printf("\n");
	
	return 0;
}

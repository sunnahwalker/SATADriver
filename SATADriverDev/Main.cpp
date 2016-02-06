///////////////////////////////////////////////////////////////////
//
//		App to check identify device and SMART data  
//		Author: Ahsan Uddin
//		© Sunnahwalker Productions
//		Version: 1.1 Release: 1/17/2016
//
///////////////////////////////////////////////////////////////////

//Global includes
#include <iostream>
#include <stdio.h>
#include <Windows.h>
 
//Local includes
#include "Api.h"
#include "SMART.h"

//Windows DDK specific includes
#include <ntddscsi.h>
#include <Ata.h>

//GLOBAL Pre Processors
//#define WIN7_COMPATIBLE

//Local Pre Processors
//#define DEBUG_PRINTS_1
#define SMART
#ifdef WIN7_COMPATIBLE
#define _WIN32_WINNT_WIN7	0x0601
#endif

//Local Defines
#define VERSION 01
#ifndef DEBUG_PRINTS_1
#define TIME_OUT 20		//20 seconds T/O
#else
#define TIME_OUT 3		//Shorter T/O in case if we are trying to debug something
#endif



int main() {

	printf("Welcome to Ahsan's SATA drive info program, v%02d\n", VERSION);
	printf("Code compiled on: %s %s\n\n", __DATE__, __TIME__);

	
	extern HANDLE Sata_Drive;

	if (find_port() == FALSE) {
		printf("No valid drives found, program will exit :(\n\n");
		exit(EXIT_FAILURE);
	}

	void *pBuffer = malloc(sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA));
	memset(pBuffer, 0, sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA));

	ATA_PASS_THROUGH_EX *pATA_Buffer =  (ATA_PASS_THROUGH_EX *) pBuffer;

	pATA_Buffer->AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
	pATA_Buffer->Length = sizeof(ATA_PASS_THROUGH_EX);
	pATA_Buffer->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);
	pATA_Buffer->DataTransferLength = sizeof(IDENTIFY_DEVICE_DATA);
	pATA_Buffer->TimeOutValue = TIME_OUT;
	pATA_Buffer->CurrentTaskFile[6] = ID_CMD;

	BOOL ret = DeviceIoControl(	Sata_Drive,
								IOCTL_ATA_PASS_THROUGH,
								pBuffer, (sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA)),
								pBuffer, (sizeof(ATA_PASS_THROUGH_EX) + sizeof(IDENTIFY_DEVICE_DATA)),
								NULL,
								NULL
								);

#if defined DEBUG_PRINTS_1
	printf("Status register of last SATA CMD: 0x%02x\n", pATA_Buffer->CurrentTaskFile[6]);
#endif

	if (ret == FALSE) {
		printf("IOCTL IDFY returned FAIL, program will exit :(\n\n");
		exit(EXIT_FAILURE);
	}


#ifndef SMART
	CloseHandle(Sata_Drive);
#endif

	IDENTIFY_DEVICE_DATA *pIDFY_Buffer;

	pIDFY_Buffer = (IDENTIFY_DEVICE_DATA *)(int(pBuffer) + sizeof(ATA_PASS_THROUGH_EX));

	printf("SKU is: ");
	sorted_print((pIDFY_Buffer->ModelNumber), 40);
	printf("\n");

	printf("Serial Number is: ");
	sorted_print((pIDFY_Buffer->SerialNumber), 20);
	printf("\n");

	printf("FW Rev is: ");
	sorted_print((pIDFY_Buffer->FirmwareRevision), 8);
	printf("\n");

	printf("Current SATA Gen is: %d\n", pIDFY_Buffer->SerialAtaCapabilities.CurrentSpeed);

	free(pBuffer);
	//free(pATA_Buffer);

#if defined SMART

	pBuffer = malloc(sizeof(ATA_PASS_THROUGH_EX) + READ_ATTRIBUTE_BUFFER_SIZE);
	memset(pBuffer, 0, sizeof(ATA_PASS_THROUGH_EX) + READ_ATTRIBUTE_BUFFER_SIZE);

	ret = FALSE; //resetting, just incase

	pATA_Buffer = (ATA_PASS_THROUGH_EX *)pBuffer;

	pATA_Buffer->AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
	pATA_Buffer->Length = sizeof(ATA_PASS_THROUGH_EX);
	pATA_Buffer->DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);
	pATA_Buffer->DataTransferLength = READ_ATTRIBUTE_BUFFER_SIZE;
	pATA_Buffer->TimeOutValue = TIME_OUT;
	
	pATA_Buffer->CurrentTaskFile[0] = READ_ATTRIBUTES;
	pATA_Buffer->CurrentTaskFile[1] = 0x01;
	//pATA_Buffer->CurrentTaskFile[2] = ;
	pATA_Buffer->CurrentTaskFile[3] = SMART_CYL_LOW;
	pATA_Buffer->CurrentTaskFile[4] = SMART_CYL_HI;
	pATA_Buffer->CurrentTaskFile[5] = 0xE0;
	pATA_Buffer->CurrentTaskFile[6] = SMART_CMD;

	ret = DeviceIoControl(Sata_Drive,
		IOCTL_ATA_PASS_THROUGH,
		pBuffer, (sizeof(ATA_PASS_THROUGH_EX) + READ_ATTRIBUTE_BUFFER_SIZE),
		pBuffer, (sizeof(ATA_PASS_THROUGH_EX) + READ_ATTRIBUTE_BUFFER_SIZE),
		NULL,
		NULL
		);

#if defined DEBUG_PRINTS_1
	printf("Status register of last SATA CMD: 0x%02x\n", pATA_Buffer->CurrentTaskFile[6]);
#endif

	if (ret == FALSE) {
		printf("IOCTL SMART returned FAIL, program will exit :(\n\n");
		exit(EXIT_FAILURE);
	}

	CloseHandle(Sata_Drive);

	parse_SMART((void *)(int(pBuffer) + sizeof(ATA_PASS_THROUGH_EX)));
	
#endif

	printf("\n");

	return 0;
}

#include <iostream>
#include <stdio.h>
#include <Windows.h>

#include "Buffer.h"

#define RAW_SMART_VALUE_SIZE	6 

#define RELOCATED_BLOCKS		5 
#define SWITCHED_ON_HOURS		9


void parse_SMART(void * SMART_Buffer) {
	
	int i;
	UINT8 * SMART_payload = (UINT8*) SMART_Buffer;

	printf("\nPrinting SMART ATTRs:\nSMART ID:\tATTR Description:\t\tVALUES:\n");

	for (i = 2; i < (READ_ATTRIBUTE_BUFFER_SIZE - 12); i += 12) {
		switch(SMART_payload[i]) {
			case SWITCHED_ON_HOURS: 
				printf("%d\t\t%s\t\t%d\n", SMART_payload[i], "SMART SWITCHED ON HOURS",	get_value(&SMART_payload[i + 5], RAW_SMART_VALUE_SIZE));
				break;
			case RELOCATED_BLOCKS:
				printf("%d\t\t%s\t\t%d\n", SMART_payload[i], "RELOCATED BLOCKS",		get_value(&SMART_payload[i + 5], RAW_SMART_VALUE_SIZE));
				break;
			default:
				if (SMART_payload[i] != 0) {
					printf("%d\t\t%s\t\t%d\n", SMART_payload[i], "ATTRIBUTE NOT FOUND",	get_value(&SMART_payload[i + 5], RAW_SMART_VALUE_SIZE));
				}
				else {
					//Manually trying to break the for loop. 
					//If we are reading 0 values, it means we are done probing for SMART ATTRs.

					i = READ_ATTRIBUTE_BUFFER_SIZE;
				}
				break;
		}
	}

	printf("\nFinished printing SMART data!!!...\n\n");

	return;
}
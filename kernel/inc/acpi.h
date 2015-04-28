#pragma once

#include <stdint.h>

void init_acpi();

#define PACKED __attribute__((packed))

//ACPI Version 1.0 
typedef struct {
	char 		Signature[8];
	uint8_t 	Checksum;
	char 		OEMID[6];
	uint8_t 	Revision;
	uint32_t 	RsdtAddress;
} PACKED RSDPDescriptor;

//ACPI Version 2.0 
typedef struct RSDPDescriptor20 {
	RSDPDescriptor 	firstPart;
	uint32_t 		Length;
	uint64_t 		XsdtAddress;
	uint8_t 		ExtendedChecksum;
	uint8_t 		reserved[3];
} PACKED RSDPDescriptor20;

typedef struct {
	char 		Signature[4];
	uint32_t 	Length;
	uint8_t 	Revision;
	uint8_t 	Checksum;
	char 		OEMID[6];
	char 		OEMTableID[8];
	uint32_t 	OEMRevision;
	uint32_t 	CreatorID;
	uint32_t 	CreatorRevision;
} PACKED ACPISDTHeader;

RSDPDescriptor *get_rsdp();























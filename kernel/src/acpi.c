#include <acpi.h>
#include <param.h>
#include <string.h>
#include <klib.h>
#include <console.h>

static size_t chksum(void *buff, size_t num) {
	size_t sum = 0;
	unsigned char *ptr = buff;
	unsigned char *end = ptr + num;
	for (; ptr < end; ptr++)
		sum += *ptr;
	return sum;
}

static int validate(RSDPDescriptor *rsdp) {
	
	size_t sum = chksum(rsdp, sizeof(RSDPDescriptor));
	sum &= 0xff;
	
	if (sum != 0) {
		kprintfln("fake ass RSDP!");
		return 0;
	}
	
	if (rsdp->Revision == 0)
		return 1;
		
	sum = chksum(rsdp, sizeof(RSDPDescriptor20));
	sum &= 0xff;
	
	if (sum != 0) {
		kprintfln("fake ass version 2 RSDP!");
		return 0;
	}
		
	return 1;
}

#define RSD_SIG		"RSD PTR "

static RSDPDescriptor *find_rsdp_in(void *from, void *to) {

	for (char *ptr = from; ptr < (char *)to; ptr += 2) {
	
		if (memcmp(ptr, RSD_SIG, 8) != 0)
			continue;
	
		if (!validate((RSDPDescriptor *)ptr))
			continue;
			
		return (RSDPDescriptor *)ptr;
	}
	
	return 0;
}

#define EBDA		((char *)(0x0009FC00 + KERNEL_OFF))
#define START		((char *)(0x000E0000 + KERNEL_OFF))
#define END			((char *)(0x000FFFFF + KERNEL_OFF))

static RSDPDescriptor *find_rsdp() {

	RSDPDescriptor *tmp = 0;
	
	tmp = find_rsdp_in(EBDA, EBDA + 1024);
	
	if (tmp == 0)
		tmp = find_rsdp_in(START, END);
		
	return tmp;
}

static ACPISDTHeader *get_rsdt(RSDPDescriptor *rsdp) {
	if (rsdp->Revision == 0)
		return (ACPISDTHeader *)rsdp->RsdtAddress;
	panic("ACPI 2 not supported!");
	return 0;
}

void init_acpi() {

	kprintfln("init acpi");

	RSDPDescriptor *rsdp = find_rsdp();
	
	if (rsdp == 0)
		panic("RSDP not found!");
		
	ACPISDTHeader *rsdt = get_rsdt(rsdp);
	kprintfln("rsdt addr: %d", (uintptr_t)rsdt / 1024);
	kprintfln("rsdt sig: %s", rsdt->Signature);
}





















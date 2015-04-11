#include <pci.h>

#include <stdint.h>
#include <io.h>
#include <console.h>
#include <string.h>
#include <pci_list.h>
#include <ide.h>

#define CONFIG_ADDRESS		(0xCF8)
#define CONFIG_DATA 		(0xCFC)

static uint32_t pci_conf_read(	uint8_t bus, uint8_t dev,
								uint8_t fun, uint8_t reg)
{
	uint32_t addr = 0;
	
	addr |= 0x80000000; //?
	addr |= (uint32_t)bus << 16;
	addr |= (uint32_t)(dev & 0b11111) << 11;
	addr |= (uint32_t)(fun & 0b111) << 8;
	addr |= (uint32_t)reg * 4;
	
	out32(CONFIG_ADDRESS, addr);
	
	return in32(CONFIG_DATA);
}

static uint8_t pci_header_type(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 3);
	return (reg >> 16) & 0xff;
}

static uint16_t pci_vendor_id(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 0);
	return reg & 0xffff;
}

static uint16_t pci_device_id(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 0);
	return (reg >> 16) & 0xffff;
}

static uint8_t pci_class(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 2);
	return (reg >> 24) & 0xff;
}

static uint8_t pci_subclass(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 2);
	return (reg >> 16) & 0xff;
}

static uint8_t pci_prog_if(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 2);
	return (reg >> 8) & 0xff;
}

static uint8_t pci_secondary_bus(uint8_t bus, uint8_t dev, uint8_t fun) {

	uint32_t reg = pci_conf_read(bus, dev, fun, 6);
	return (reg >> 8) & 0xff;
}

static void scan_bus(uint8_t bus);

static void dump_device(uint16_t vendor_id, uint16_t device_id) {
	
	for (size_t i = 0; i < PCI_DEVTABLE_LEN; i++) {
		PCI_DEVTABLE *d = &PciDevTable[i];
		if (d->VenId == vendor_id && d->DevId == device_id) {
			kprintf("- %s / ", d->ChipDesc);
			return;
		}
	}
	
	kprintf("- unknown / ");
}

static void dump_class(uint8_t class, uint8_t subclass, uint8_t prog_if) {
		
	//IDE exception
	if (class == 0x1 && subclass == 0x1)
		prog_if = 0x0;
						
	for (size_t i = 0; i < PCI_CLASSCODETABLE_LEN; i++) {
						
		PCI_CLASSCODETABLE *c = &PciClassCodeTable[i];
		
		if (	c->BaseClass == class && 
				c->SubClass == subclass &&
				c->ProgIf == prog_if)
		{
			kprintfln(	"%s | %s | %s",
						c->BaseDesc,
						c->SubDesc,
						c->ProgDesc);
			return;
		}
	}
	
	kprintfln("unknown.");
}

#define C_IDE	0x1
#define S_IDE	0x1

static int bar0(int bus, int dev, int fun) {
	return pci_conf_read(bus, dev, fun, 4);
}

static int bar1(int bus, int dev, int fun) {
	return pci_conf_read(bus, dev, fun, 5);
}

static int bar2(int bus, int dev, int fun) {
	return pci_conf_read(bus, dev, fun, 6);
}

static int bar3(int bus, int dev, int fun) {
	return pci_conf_read(bus, dev, fun, 7);
}

static int bar4(int bus, int dev, int fun) {
	return pci_conf_read(bus, dev, fun, 8);
}

static void scan_function(uint8_t bus, uint8_t dev, uint8_t fun) {
	
	uint16_t vendor_id = pci_vendor_id(bus, dev, fun);
	uint16_t device_id = pci_device_id(bus, dev, fun);
	
	dump_device(vendor_id, device_id);
	
	uint8_t class_code = pci_class(bus, dev, fun);
	uint8_t subclass_code = pci_subclass(bus, dev, fun);
	uint8_t prog_if = pci_prog_if(bus, dev, fun);
	
	dump_class(class_code, subclass_code, prog_if);

	if (class_code == C_IDE && subclass_code == S_IDE) {
	
		int r0 = bar0(bus, dev, fun);
		int r1 = bar1(bus, dev, fun);
		int r2 = bar2(bus, dev, fun);
		int r3 = bar3(bus, dev, fun);
		int r4 = bar4(bus, dev, fun);
		
		init_ide(r0, r1, r2, r3, r4);
	}
	
	if (class_code == 0x6 && subclass_code == 0x4) {
		//bridge
		kprintfln("! bridge function");
		uint8_t sbus = pci_secondary_bus(bus, dev, fun);
		scan_bus(sbus);
	}
}

static void dump_vendor(uint16_t vendor_id) {
	
	for (size_t i = 0; i < PCI_VENTABLE_LEN; i++) {
		PCI_VENTABLE *v = &PciVenTable[i];
		if (v->VenId == vendor_id) {
			kprintfln("%s", v->VenFull);
			return;
		}
	}
	
	kprintfln("unknown");
}

static void scan_device(uint8_t bus, uint8_t dev) {

	uint16_t vendor_id = pci_vendor_id(bus, dev, 0);
	
	if (vendor_id == 0xffff)
		return;
		
	dump_vendor(vendor_id);
		
	scan_function(bus, dev, 0);
	
	uint8_t header_type = pci_header_type(bus, dev, 0);
	
	if ((header_type & 0x80) != 0) {
		//multiple function device
		//kprintfln("multiple function device");
		for (int i = 1; i < 8; i++) {
			if (pci_vendor_id(bus, dev, i) != 0xffff) {
				scan_function(bus, dev, i);
			}
		}
	}
	
	kprintfln("");
}

static void scan_bus(uint8_t bus) {
	for (int dev = 0; dev < 32; dev++) {
		scan_device(bus, dev);
	}
}

static void scan_all_buses() {

	uint8_t header_type = pci_header_type(0, 0, 0);
	
	if ((header_type & 0x80) == 0) {
		//single host controller
		kprintfln("single host controller");
		kprintfln("");
		scan_bus(0);
	} else {
		//multiple host controllers
		kprintfln("multiple host controllers");
		kprintfln("");
		for (int i = 0; i < 8; i++) {
			if (pci_vendor_id(0, 0, i) != 0xffff) {
				scan_bus(i);
			}
		}
	}
}

void init_pci() {

	scan_all_buses();
}






















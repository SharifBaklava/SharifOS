#pragma once

#include <stdint.h>
#include <stdio.h>

#define GDT_ENTRY_SIZE 6
extern "C" void setGdt(void *gdtr_ptr);

struct GDTEntry
{
	uint16_t limit_low;	 // Bits 0–15 of segment limit
	uint16_t base_low;	 // Bits 0–15 of base address
	uint8_t base_mid;	 // Bits 16–23 of base address
	uint8_t access;		 // Access flags
	uint8_t granularity; // Granularity + high 4 bits of limit
	uint8_t base_high;	 // Bits 24–31 of base address
} __attribute__((packed));

struct GDTDescriptor
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void set_gdt_entry(GDTEntry *entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

class GDT
{
public:
	GDTEntry gdt[5];
	GDTDescriptor gdtdesc;
	void init()
	{
		set_gdt_entry(&gdt[0], 0, 0, 0, 0);
		set_gdt_entry(&gdt[1], 0, 0xFFFFF, 0x9A, 0xCF); // Code segment
		set_gdt_entry(&gdt[2], 0, 0xFFFFF, 0x92, 0xCF); // Data segment
		set_gdt_entry(&gdt[3], 0, 0x000FFFFF, 0xfa, 0xcf);
		set_gdt_entry(&gdt[4], 0, 0x000FFFFF, 0xf2, 0xcf);
		gdtdesc = {sizeof(gdt) - 1, (uint32_t)gdt};
		lgdt();
		printf("gdt %x\n", gdtdesc.base);
	}
	inline void lgdt()
	{
		setGdt(&gdtdesc);
	}
};
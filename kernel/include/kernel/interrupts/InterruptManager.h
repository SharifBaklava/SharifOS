#pragma once
#include <stdint.h>
#include <stdlib.h>

#define IDT_PAGES 1

extern "C" void *isr_stub_table[];

typedef struct InterruptDescriptor32
{
	uint16_t offset_1;		 // offset bits 0..15
	uint16_t selector;		 // a code segment selector in GDT or LDT
	uint8_t zero;			 // unused, set to 0
	uint8_t type_attributes; // gate type, dpl, and p fields
	uint16_t offset_2;		 // offset bits 16..31
} __attribute__((packed)) idt_entry_t;

typedef struct
{
	uint32_t offset;
	uint16_t size;
} __attribute__((packed)) idt_descriptor_t;

#define DEFINE_ISR(name)                                 \
	extern "C" void name();                              \
	extern "C" __attribute__((naked)) void name##_stub() \
	{                                                    \
		asm volatile(                                    \
			"call " #name "\n\t"                         \
			"iret\n\t");                                 \
	}                                                    \
	void name()



class InterruptManager
{
	// idt_entry_t idt[256];
	idt_entry_t* idt;

	idt_descriptor_t idt_desc;

public:
	void init();
	void fill_idt();
	void set_idt_entry(uint8_t idx, uint32_t offset, uint16_t selector, uint8_t gate_type, uint8_t dpl, bool present);
	void load_idt();
};

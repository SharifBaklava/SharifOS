#include <kernel/interrupts/InterruptManager.h>
#include <kernel/kernel.h>
#include <stdio.h>


DEFINE_ISR(zero_devision)
{
	printf("Zero Division Exception occurred!\n");
	abort();
}


DEFINE_ISR(default_f)
{
    printf("Unhandled interrupt!\n");
}

__attribute__((noreturn)) extern "C" void exception_handler()
{
	printf("Exception occurred! System halted.\n");
	abort();
}

void InterruptManager::init()
{
	idt = (idt_entry_t*)krn.memoryManager.pagingManager.allocate(1);
	if (!idt)
	{
		printf("Failed to allocate memory for IDT.\n");
		abort();
	}
	fill_idt();
	printf("Initializing Interrupt Manager... %x\n", idt);
	load_idt();
}
void InterruptManager::fill_idt()
{
    for (uint16_t vector = 0; vector < 256; vector++)
    {
        set_idt_entry(vector, (uint32_t)&default_f_stub, 0x08, 0xE, 0, true);
    }
}
void InterruptManager::set_idt_entry(uint8_t idx, uint32_t offset, uint16_t selector, uint8_t gate_type, uint8_t dpl, bool present)
{
	idt_entry_t &entry = idt[idx];
	entry.offset_1 = offset & 0xFFFF;														 // Lower 16 bits of the offset
	entry.selector = selector;																 // Code segment selector in GDT or LDT
	entry.zero = 0;																			 // Reserved, must be zero
	entry.type_attributes = (gate_type & 0x0F) | ((dpl & 0x03) << 5) | (present ? 0x80 : 0); // Gate type, DPL, and P fields
	entry.offset_2 = (offset >> 16) & 0xFFFF;												 // Upper 16 bits of the offset
}
void InterruptManager::load_idt()
{
	printf("Loading IDT... %x\n", idt);
	idt_desc.offset = (uint32_t)idt;
	idt_desc.size = 256*8 - 1;
	asm volatile("lidt %0" : : "m"(idt_desc));
	asm volatile("sti" : :);

}


#include <stdio.h>
#include <cstdint>
#include <kernel/memory/memorymanager.h>
#include <kernel/kernel.h>

Kernel Kernel::i;

void Kernel::initialize(uintptr_t p_multiboot_info)
{
	gdtInitializer.init();
	terminal.initialize();
	memoryManager.init(p_multiboot_info);
	printf("welcome to SharifOS \n");

}
extern "C" void kernel_main(uint32_t magic, uint32_t mbi_addr)
{

	krn.initialize(mbi_addr);
}

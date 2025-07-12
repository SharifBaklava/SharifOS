#include <stdio.h>
#include <cstdint>
#include <kernel/memory/memorymanager.h>
#include <kernel/kernel.h>

Kernel Kernel::i;

void Kernel::initialize(uintptr_t p_multiboot_info)
{
	terminal.initialize();
	memoryManager.init(p_multiboot_info);
	
	// printf("welcome to SharifOS \n");
	// printf("Memory start address: %x\n", memoryManager.ul_memory_start);
	// printf("Total RAM: %x\n", (memoryManager.ul_physical_memory_size)/(1024 * 1024));

	// printf("Kernel size: %x\n", memoryManager.ul_kernel_size);
	// printf("Kernel start address: %x\n", memoryManager.p_kernel_start);
}
extern "C" void kernel_main(uint32_t magic, uint32_t mbi_addr)
{

	krn.initialize(mbi_addr);
}

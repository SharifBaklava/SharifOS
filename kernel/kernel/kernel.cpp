#include <stdio.h>
#include <cstdint>
#include <kernel/memory/memorymanager.h>
#include <kernel/kernel.h>


Kernel Kernel::i;


extern "C" void kernel_main(uint32_t magic, uint32_t mbi_addr)
{

	Kernel::i.terminal.initialize();
	printf("welcome to SharifOS \n");
	uint32_t total_ram_mb = get_ram_size(mbi_addr);

	printf("Total RAM: %x\n",total_ram_mb);


}

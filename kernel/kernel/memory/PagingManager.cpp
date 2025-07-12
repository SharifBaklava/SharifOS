#include <kernel/kernel.h>
#include <kernel/memory/PagingManager.h>




void PagingManager::init()
{

	pageDirectory = (PageDirectoryEntry *)krn.memoryManager.buddy_allocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	if (pageDirectory == nullptr)
	{
        printf("you murdered the kernel :(\n");
	}
	PageTableEntry *pageTable = (PageTableEntry *)krn.memoryManager.buddy_allocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	for (int i = 0; i < 1024; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		pageTable[i].value = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}
	pageDirectory[0] = PageDirectoryEntry {.value = (uint32_t)pageTable | 3};


}


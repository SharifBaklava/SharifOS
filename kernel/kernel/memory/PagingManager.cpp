#include <kernel/kernel.h>
#include <kernel/memory/PagingManager.h>

void PagingManager::init()
{
	pageDirectory = (PageDirectoryEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	if (pageDirectory == nullptr)
	{
		printf("you murdered the kernel :(\n");
	}
	for (size_t i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	{
		pageDirectory[i].value = 0;
	}

	// TODO: delete pagetable allocations from init
	PageTableEntry *pageTable = (PageTableEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		pageTable[i].value = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}
	pageDirectory[0] = PageDirectoryEntry{.value = (uint32_t)pageTable | 3};
}

size_t find_free_pages(size_t num, size_t *dir_entry, size_t *table_entry)
{
	// size_t count = 0;
	// for (size_t i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	// {
	// 	PageTableEntry *pageTable = (PageTableEntry *)(pageDirectory[i].bits.page_kind.david.phys_addr << 12);
	// 	for (size_t j = 0; j < PAGE_TABLE_ENTRIES; j++)
	// 	{
	// 		if (pageTable[j].bits.present)
	// 		{
	// 			if (++count == num)
	// 			{
	// 				*table_entry = j - count % PAGE_TABLE_ENTRIES + 1;
	// 				*dir_entry = i - count / PAGE_TABLE_ENTRIES;
	// 				if (j < num)
	// 					*dir_entry--;
	// 				if (table_entry < 0)
	// 					*table_entry += PAGE_TABLE_ENTRIES;
	// 				// PageTableEntry *page_target = (PageTableEntry *)(pageDirectory[dir_entry].bits.page_kind.david.phys_addr << 12);
	// 				return num;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			count = 0;
	// 		}
	// 	}
	// }
	return 0;
}

void *allocate(size_t size)
{
	// size_t pages_num = (size + BLOCK_SIZE - 1) >> 12;
	// size_t dir_entry, table_entry;
	// size_t new_address = find_free_pages(size, &dir_entry, &table_entry);
	// if (new_address == 0)
	// 	return nullptr;
	
	// size_t i = 0;
	// for (; i < pages_num; i++)
	// {
	// 	void *ptr = krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	// 	if (ptr == nullptr)
	// 	 	break;
	// 	PageTableEntry *page = (PageTableEntry *)(pageDirectory[dir_entry].bits.page_kind.david.phys_addr << 12);
	// 	page[table_entry + i] = PageTableEntry{.value = (uint32_t)ptr | 3};	
	// 	// TODO: implement
	// }
	// // find size/4k free entries
	// // map the physical address to a virtual address
	// return new_address;
	return nullptr;
}
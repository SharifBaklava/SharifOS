#include <kernel/kernel.h>
#include <kernel/memory/PagingManager.h>
#include <stdlib.h>
void PagingManager::init()
{
	pageDirectory = (PageDirectoryEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	if (pageDirectory == nullptr)
	{
		abort();
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

size_t PagingManager::find_free_pages(size_t num, size_t *dir_entry_idx, size_t *table_entry_idx)
{
	size_t count = 0;
	size_t i = 0;
	size_t j = 0;
	bool success = false;
	for (i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	{
		if (success)
			break;
		PageTableEntry *pageTable = (PageTableEntry *)(pageDirectory[i].bits.page_kind.david.phys_addr << 12);
		if (pageTable->value == 0)
		{
			// Page Not present
			if (count + PAGE_TABLE_ENTRIES >= num)
			{
				success = true;
				break;
			}
		}
		else
		{
			for (j = 0; j < PAGE_TABLE_ENTRIES; j++)
			{
				if (pageTable[j].value == 0)
				{
					if (++count == num)
					{
						success = true;
						break;
					}
				}
				// Page present
				else
					count = 0;
			}
		}
	}

	if (success)
	{
		*dir_entry_idx = i - count / PAGE_TABLE_ENTRIES;
		if (j < num)
			*dir_entry_idx--;

		// if (*table_entry_idx < 0)
		if (j < count % PAGE_TABLE_ENTRIES + 1)
			*table_entry_idx += PAGE_TABLE_ENTRIES;

		*table_entry_idx = j - count % PAGE_TABLE_ENTRIES + 1;
		return num;
	}
	else
	{
		*dir_entry_idx = 0;
		*table_entry_idx = 0;
		return 0;
	}
}

int PagingManager::create_page_table(size_t dir_entry_idx)
{
	void *ptr = krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	if (ptr == nullptr)
		return -1;
	pageDirectory[dir_entry_idx].value = ((uint32_t)ptr | 3);
	for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
	{
		((PageTableEntry *)ptr)[i].value = 0;
	}
	return 0;
}

int PagingManager::create_page_entry(PageTableEntry *table_entry, size_t table_entry_idx)
{
	void *ptr = krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	if (ptr == nullptr)
		return -1;
	table_entry[table_entry_idx].value = ((uint32_t)ptr | 3);
	return 0;
}
void *PagingManager::allocate(size_t num_pages)
{
	// size_t pages_num = (num_pages + BLOCK_SIZE - 1) >> 12;
	size_t dir_entry_idx, table_entry_idx;
	size_t num_allocated = find_free_pages(num_pages, &dir_entry_idx, &table_entry_idx);
	size_t dir_entry_target = dir_entry_idx, table_entry_target = table_entry_idx;

	if (num_allocated == 0)
		return nullptr;
	size_t i = 0;
	for (; i < num_pages; i++)
	{
		if (table_entry_idx + i >= PAGE_TABLE_ENTRIES)
		{
			dir_entry_idx++;
			table_entry_idx = 0;
		}
		else
			table_entry_idx++;

		PageDirectoryEntry dirEntry = pageDirectory[dir_entry_idx];
		if (dirEntry.value == 0)
		{
			if (create_page_table(dir_entry_idx)!=0)
				break;
		}

		if (create_page_entry((PageTableEntry *)dirEntry.bits.page_kind.david.get_addr(), table_entry_idx)!=0)
			break;
	}

	// TODO: free if i < pages_num
	if (i < num_pages)
	{
		for (; i < num_pages; i++)
		{
			// TODO: free()
			break;
		}
		return nullptr;
	}

	// find num_pages/4k free entries
	// map the physical address to a virtual address
	return get_page_addr(dir_entry_target, table_entry_target);
}

int PagingManager::free_page_entry(PageTableEntry *table_entry, size_t table_entry_idx)
{
	if (table_entry[table_entry_idx].value == 0)
		return -1;
	if (!krn.memoryManager.buddyAllocator.free((void *)(table_entry[table_entry_idx].bits.phys_addr << 12)))
		return -1;
	table_entry[table_entry_idx].value = 0;
	return 0;
}
bool PagingManager::page_table_is_free(PageTableEntry *addr)
{
	for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
	{
		if (addr[i].value != 0)
			return false;
	}
	return true;
}

int PagingManager::free_page_table(size_t dir_entry_idx)
{

	void *addr = (void *)(pageDirectory[dir_entry_idx].bits.page_kind.david.get_addr());
	if (page_table_is_free((PageTableEntry *)addr))
	{
		if (!krn.memoryManager.buddyAllocator.free(addr))
			return -1;
		pageDirectory[dir_entry_idx].value = 0;
		return 0;
	}
}

void PagingManager::free(void *addr, size_t num_pages)
{
	size_t dir_entry_idx = get_dir_idx(addr);
	size_t table_entry_idx = get_table_idx(addr);

	size_t i = 0;
	for (; i < num_pages; i++)
	{
		if (table_entry_idx + i >= PAGE_TABLE_ENTRIES)
		{
			// TODO: should we free the page table?
			// free_page_table(dir_entry_idx);
			dir_entry_idx++;
			table_entry_idx = 0;
		}
		else
			table_entry_idx++;

		PageDirectoryEntry dirEntry = pageDirectory[dir_entry_idx];

		if (!free_page_entry(dirEntry.bits.page_kind.david.get_addr(), table_entry_idx))
		{
			// TODO: panic
		}
	}
}

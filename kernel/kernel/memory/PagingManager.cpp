#include <kernel/kernel.h>
#include <kernel/memory/PagingManager.h>
#include <stdlib.h>

// Macro to extract Page Directory Index (bits 31–22)
#define PAGE_DIRECTORY_INDEX(va) (((size_t)(va) >> 22) & 0x3FF)

// Macro to extract Page Table Index (bits 21–12)
#define PAGE_TABLE_INDEX(va) (((size_t)(va) >> 12) & 0x3FF)

void *PagingManager::new_page()
{
	void *ptr = krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	memset(ptr, 0, BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	return ptr;
}
void PagingManager::init()
{
	pageDirectory = (PageDirectoryEntry *)new_page();
	pageDirectoryVirtual = pageDirectory;
	if (pageDirectory == nullptr)
	{
		abort();
	}
	for (size_t i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
	{
		pageDirectory[i].value = 0;
	}

	void *start = (void *)(krn.memoryManager.p_kernel_start);
	void *end = (void *)((size_t)krn.memoryManager.buddyAllocator.m_p_block_info_array_start + krn.memoryManager.buddyAllocator.m_ui_block_info_array_size);
	size_t i = PAGE_DIRECTORY_INDEX(start);
	size_t j = PAGE_TABLE_INDEX(start);

	printf("start reserved: %x\nend reserved: %x\n", start, end);
	// PageTableEntry *pageTable = (PageTableEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	PageTableEntry *pageTable = (PageTableEntry *)allocate(1);
	pageDirectory[i].value = (uint32_t)pageTable | 3;
	while (start < end)
	{

		if (j >= PAGE_TABLE_ENTRIES)
		{
			i++;
			j = 0;
			// pageTable = (PageTableEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
			pageTable = (PageTableEntry *)allocate(1);
			pageDirectory[i].value = (uint32_t)pageTable | 3;
		}
		pageTable[j].value = (uint32_t)start | 3;
		printf("Reserved page: %x mapped to %x, %d, %d\n", start, i * 1024 + j * 4096, i, j);
		start = (void *)((size_t)start + 0x1000);
		j++;
	}
	start = (void *)(0xB8000); // vga start
	end = (void *)(0xB8000 + 0x1000); // vga end
	i = PAGE_DIRECTORY_INDEX(start);
	j = PAGE_TABLE_INDEX(start);
	while (start < end)
	{
		if (j >= PAGE_TABLE_ENTRIES)
		{
			i++;
			j = 0;
			pageTable = (PageTableEntry *)allocate(1);
			pageDirectory[i].value = (uint32_t)pageTable | 3;
		}
		pageTable[j].value = (uint32_t)start | 3;
		printf("Reserved page: %x mapped to %x, %d, %d\n", start, i * 1024 + j * 4096, i, j);
		start = (void *)((size_t)start + 0x1000);
		j++;
	}

	// find_free_pages(1, &i, &j);
	// PageTableEntry *te = (PageTableEntry *)pageDirectory[i].bits.page_kind.david.get_addr();
	// te[j].value = ((uint32_t)pageDirectory | 3);

	pageDirectory[1023].value = (uint32_t)pageDirectory | 3; // Map the last page directory entry to itself
	
	pageDirectoryVirtual = 0xffc00000;
	printf("Found free pages at %d, %d\n", i, j);

	// // Reserve memory for the kernel within the block info array

	// // TODO: delete pagetable allocations from init
	// PageTableEntry *pageTable = (PageTableEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	// for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
	// {
	// 	// As the address is page aligned, it will always leave 12 bits zeroed.
	// 	// Those bits are used by the attributes ;)
	// 	pageTable[i].value = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	// }
	// pageDirectory[0] = PageDirectoryEntry{.value = (uint32_t)pageTable | 3};
	// PageTableEntry *pageTable2 = (PageTableEntry *)krn.memoryManager.buddyAllocator.allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	// pageTable2[0].value = (uint32_t)pageDirectory | 3;
	// pageTable2[1].value = (uint32_t)pageTable | 3;
	// pageTable2[2].value = (uint32_t)pageTable2 | 3; // Map the second page table to itself

	// pageDirectory[1] = PageDirectoryEntry{.value = (uint32_t)pageTable2 | 3};

	// pageDirectory = (PageDirectoryEntry*)(1024*4096);
}
#define PAGE_DIRECTORY_VIRTUAL_ADDR 0xfffff000
#define PAGE_TABLE_VIRTUAL_ADDR(idx) ((PageDirectoryEntry*)(0xffc00000 + (idx << 12)))
#define PAGE_TABLE_ENTRY_VIRTUAL_ADDR(dir_idx, table_idx) ((PageTableEntry*)(PAGE_TABLE_VIRTUAL_ADDR(dir_idx) + (table_idx << 2)))

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
		printf("Checking directory entry %d\n", i);
		printf("Page Directory Entry2222: %x\n", pageDirectoryVirtual);
		PageDirectoryEntry *dirEntry = &pageDirectoryVirtual[i];
		PageTableEntry *pageTable = (PageTableEntry *)(dirEntry->bits.page_kind.david.phys_addr << 12);
		if (dirEntry->value == 0)
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
	void *ptr = new_page();
	if (ptr == nullptr)
		return -1;
	pageDirectoryVirtual[dir_entry_idx].value = ((uint32_t)ptr | 3);
	for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++)
	{
		((PageTableEntry *)ptr)[i].value = 0;
	}
	return 0;
}

int PagingManager::create_page_entry(PageTableEntry *table_entry, size_t table_entry_idx)
{
	void *ptr = new_page();
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
	printf("dir_entry_idx: %x, table_entry_idx: %x, num_allocated: %d\n", dir_entry_idx, table_entry_idx, num_allocated);
	// abort();
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

		PageDirectoryEntry dirEntry = pageDirectoryVirtual[dir_entry_idx];
		if (dirEntry.value == 0)
		{
			if (create_page_table(dir_entry_idx) != 0)
				break;
		}

		if (create_page_entry((PageTableEntry *)dirEntry.bits.page_kind.david.get_addr(), table_entry_idx) != 0)
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

	void *addr = (void *)(pageDirectoryVirtual[dir_entry_idx].bits.page_kind.david.get_addr());
	if (page_table_is_free((PageTableEntry *)addr))
	{
		if (!krn.memoryManager.buddyAllocator.free(addr))
			return -1;
		pageDirectoryVirtual[dir_entry_idx].value = 0;
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

		PageDirectoryEntry dirEntry = pageDirectoryVirtual[dir_entry_idx];

		if (!free_page_entry(dirEntry.bits.page_kind.david.get_addr(), table_entry_idx))
		{
			// TODO: panic
		}
	}
}

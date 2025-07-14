#include <kernel/memory/memorymanager.h>
#include <kernel/memory/BuddyAllocator.h>
#include <string.h>
#include <kernel/kernel.h>

inline void *page::get_block_start()
{
	return (void *)(((size_t)this - (size_t)krn.memoryManager.buddy_allocator.block_info_array_start) / sizeof(page) * BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
}
inline bool page::is_free()
{
	return !(flags & 0x01);
}

inline void page::set_free()
{
	flags &= 0xFE;
}
inline void page::set_inuse()
{
	flags |= 0x01;
}

/* reserving memory of kernel image and struct pages by allocating this range of memory*/
void BuddyAllocator::reserved_memory()
{
	void *start = 0; // get_page_header_by_address((void*)krn.memoryManager.p_kernel_start);
	// page* end = get_page_header_by_address((void*)(block_info_array_start + block_info_array_size));
	void *end = (void *)(block_info_array_start + block_info_array_size) - -BUDDY_ALLOCATOR_MIN_BLOCK_SIZE;

	// size_t size = end - start;
	while (start < end)
	{
		start = allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	}
}

void BuddyAllocator::init()
{
	block_info_array_start = (void *)(ALIGN_UP_4K(
		((size_t)(krn.memoryManager.p_kernel_start) + krn.memoryManager.ul_kernel_size)));

	block_info_array_size = (ALIGN_UP_4K((krn.memoryManager.ul_physical_memory_size / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE) * sizeof(page)));

	memset((void *)block_info_array_start, 0, block_info_array_size); // Should be at MemoryManager

	// TODO: take care of residue
	// get order
	uint8_t order = find_closest_lower_order(krn.memoryManager.ul_physical_memory_size);
	size_t start_addr = (size_t)krn.memoryManager.ul_memory_start;
	page *base;
	int offset = 0;
	for (order; order > 0; order--)
	{
		int step = num_of_page_headers_to_next(order);
		bool first = true;
		int page_size = get_block_size_by_order(order);
		while (true)
		{
			if (start_addr + page_size > (size_t)krn.memoryManager.ul_memory_start + krn.memoryManager.ul_physical_memory_size)
			{
				break;
			}
			base = ((page *)block_info_array_start) + offset;
			page *prev = nullptr;
			if (!first)
			{
				prev = base - step;
				prev->lru.next = (void *)base;
			}
			else
			{
				orders[order] = base;
				first = false;
			}
			base->lru.prev = (void *)prev;
			start_addr += page_size;
			offset += step;
		}
	}
	reserved_memory();

	printf("start reserved: %x\nend reserved: %x\n", krn.memoryManager.p_kernel_start, block_info_array_start + block_info_array_size);
	void *SharifNullPageHaram = allocate(BUDDY_ALLOCATOR_MIN_BLOCK_SIZE - 1);
	for (size_t o = 0; o < BUDDY_ORDERS; o++)
	{
		printf("BuddyAllocator::init: orders[%d] = 0x%d\n", o, get_free_blocks_len(o));
	}

	void *A = allocate(0x100);
	void *B = allocate(0x100);

	printf("BuddyAllocator::init: orders[%d] = 0x%d\n", 4, get_free_blocks_len(4));
	void *C = allocate(0x10000);
	printf("BuddyAllocator::init: allocated at 0x%x\n", A);

	printf("BuddyAllocator::init: allocated at 0x%x\n", B);
	printf("BuddyAllocator::init: allocated at 0x%x\n", C);

	free(B);
	A = allocate(0x10000);
	printf("BuddyAllocator::init: allocated at 0x%x\n", A);

	// 	printf("BuddyAllocator::init: order = %d\n", order);
	// 	printf("BuddyAllocator::init: block_info_array_start = 0x%x\n", block_info_array_start);
	// 	printf("BuddyAllocator::init: buddy_structs_size = 0x%x\n", block_info_array_size);
	// 	size_t buddy_blocks_num = block_info_array_size / sizeof(page); // DELETE

	// 	printf("BuddyAllocator::init: budy_blocks_num = %d\n", buddy_blocks_num);

	// 	printf("BuddyAllocator::init: next block = 0x%x\n", (size_t)orders[1]);
	// 	void *all = allocate(0x1009);
	// 	printf("BuddyAllocator::init: allocated 0x%x\n", (size_t)all);

	// 	page *p = ((page *)block_info_array_start + ((size_t)all / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE));
	// 	printf("BuddyAllocator::init: p = 0x%x, order = %d, next = 0x%x, prev = 0x%x\n", p, p->order, p->lru.next, p->lru.prev);
	// 	free(all);
	// 	printf("BuddyAllocator::init: freed 0x%x, next = 0x%x, prev = 0x%x\n", all, p->lru.next, p->lru.prev);
}

bool BuddyAllocator::free(void *addr)
{

	if (addr == nullptr)
		return false;

	if ((size_t)addr != ALIGN_UP_4K((size_t)addr))
		return false;

	page *buddy_header_ptr = get_page_header_by_address(addr);

	// Check if the address is within the block_info_array
	if ((size_t)buddy_header_ptr > (size_t)block_info_array_start + block_info_array_size || (size_t)buddy_header_ptr < (size_t)block_info_array_start)
		return false;

	if (buddy_header_ptr->is_free())
		return false;

	uint8_t order = buddy_header_ptr->order;
	mark_block_unused(buddy_header_ptr, order);

	return true; // Successfully freed the block
}

void BuddyAllocator::mark_block_unused(page *page_header, uint8_t order)
{
	for (order; order < BUDDY_ORDERS; order++)
	{
		page *aligned_buddy;
		page *buddy;

		if (IS_ALIGNED(page_header->get_block_start(), (BUDDY_ALLOCATOR_MIN_BLOCK_SIZE << order + 1)))
		{
			// buddy = second ; aligned = first = page_header
			buddy = page_header + num_of_page_headers_to_next(order);
			aligned_buddy = page_header;
		}
		else
		{
			// aligned_buddy = buddy = page_header - num_of_page_headers_to_next(order); = first  != page_header ; b1|b2
			buddy = page_header - num_of_page_headers_to_next(order);
			aligned_buddy = buddy;
		}

		if (buddy->is_free())
		{
			remove_node_from_list(buddy, order);
			// order++;
			page_header = aligned_buddy; // Update the block to the first one
		}
		else
		{
			add_node_to_list(page_header, order);
			return; // No buddy found, stop merging
		}
	}

	add_node_to_list(page_header, order);
}

void *BuddyAllocator::allocate(size_t size)
{
	uint8_t order = find_closest_upper_order(size);
	size_t count = 0;
	for (order; order < BUDDY_ORDERS; order++)
	{

		page *block = orders[order];

		if (block != nullptr)
		{
			// printf("order: %d, count: %d page_header: 0x%x\n", order, count, block);
			mark_block_used(block, order, count);
			return block->get_block_start();
		}
		count++;
	}
	// printf("error\n");
	return nullptr; // No suitable block found
}

void *BuddyAllocator::allocate(size_t size, void *preferred_addr)
{
// 	if (preferred_addr == nullptr)
// 		return allocate(size);

// 	page *page_header = get_page_header_by_address(preferred_addr);
// 	while (page_header->lru.next == nullptr && page_header->lru.prev == nullptr) // not a good condition
// 	{
// 		page_header -= 1;
// 		// iterate pages by index, until a pge that on the free list found
// 	}
// 	// get the order of the page
// 	if (!page_header->is_free())
// 		return nullptr;

// 	page *which_order = page_header;
// 	uint8_t order = -1;
// 	// find its head
// 	while (which_order->lru.prev == nullptr)
// 	{
// 		which_order = (page *)which_order->lru.prev;
// 	}
// 	for (int i = 0; i < BUDDY_ORDERS; i++)
// 	{
// 		if (which_order == orders[i])
// 		{
// 			order = i;
// 			break;
// 		}
// 	}
// 	if (order == -1)
// 		return nullptr;

// 	void *end_preferred_addr = (void *)((size_t)preferred_addr + size);
// 	while (true)
// 	{
// 		void *start_addr = page_header->get_block_start();
// 		void *end_addr = (void *)((size_t)start_addr + get_block_size_by_order(order));

// 		if (start_addr <= preferred_addr && end_addr >= end_preferred_addr)
// 		{
// 			remove_node_from_list()
// 			mark_block_used(page_header, order, 0);
// 			return start_addr;
// 		}
// 	}
// 	return nullptr;
}
int BuddyAllocator::get_free_blocks_len(uint8_t order)
{
	int count = 0;
	page *block = orders[order];
	while (block != nullptr)
	{

		count++;
		block = (page *)block->lru.next;
	}
	return count;
}

size_t BuddyAllocator::get_block_size_by_order(uint8_t order)
{
	return BUDDY_ALLOCATOR_MIN_BLOCK_SIZE << order;
}

void BuddyAllocator::mark_block_used(page *page_header, uint8_t order, size_t splits)
{
	remove_node_from_list(page_header, order);
	for (size_t i = 0; i < splits; i++)
	{
		order--;
		add_node_to_list(page_header + num_of_page_headers_to_next(order), order);
	}
}

void BuddyAllocator::add_node_to_list(page *page_header, uint8_t order)
{
	page *nextNode = orders[order];
	page_header->lru.next = (void *)nextNode;
	page_header->lru.prev = nullptr;
	nextNode->lru.prev = (void *)page_header;
	orders[order] = page_header;
	page_header->set_free();
}

void BuddyAllocator::remove_node_from_list(page *page_header, uint8_t order)
{
	page *fd = (page *)page_header->lru.next;
	page *bk = (page *)page_header->lru.prev;
	if (fd == nullptr && bk == nullptr)
		orders[order] = nullptr;
	else
	{
		if (bk == nullptr)
			orders[order] = fd; // Update the head of the list
		else
			bk->lru.next = fd;
		if (fd != nullptr)
			fd->lru.prev = bk;
	}
	page_header->lru.next = nullptr;
	page_header->lru.prev = nullptr;
	page_header->set_inuse();
	page_header->order = 0; // Reset the order of the block

	page_header->order = order;
}
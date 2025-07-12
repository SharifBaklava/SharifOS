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
	return lru.next != nullptr && lru.prev != nullptr;
}

void BuddyAllocator::init(MemoryManager *memoryManager)
{
	block_info_array_start = (void *)(ALIGN_UP_4K(
		((size_t)(memoryManager->p_kernel_start) + memoryManager->ul_kernel_size)));
	block_info_array_size = (ALIGN_UP_4K((memoryManager->ul_physical_memory_size / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE) * sizeof(page)));

	memset((void *)block_info_array_start, 0, block_info_array_size); // Should be at MemoryManager

	// TODO: take care of residue
	// get order
	uint8_t order = find_closest_lower_order(memoryManager->ul_physical_memory_size);
	size_t i = 0;
	size_t mmmmm = 0;
	for (order; order > 0; order--)
	{
		bool start = true;
		size_t order_page_header_offset = num_of_page_headers_to_next(order) * sizeof(page);
		printf("BuddyAllocator::init: order = %d, order_page_header_offset = %d\n", order, order_page_header_offset);
		while (i < block_info_array_size)
		{
	
			page *page_header = (page *)(block_info_array_start + i);
			void *prev_block = NULL;
			if (!start)
			{
				prev_block = (void *)(block_info_array_start + i - order_page_header_offset);
				((page*)prev_block)->lru.next = (void *)page_header; // Link the previous block to the current one
			}
			else
			{
				orders[order] = page_header; // Initialize the first block in the largest order
				start = false;
			}
	
			// void *next_block = (void *)(i + order_page_header_offset >= block_info_array_size ? NULL : (block_info_array_start + i + order_page_header_offset));
			page_header->flags = 0; // Initialize flags to 0
			page_header->lru.prev = (void *)prev_block;
			page_header->lru.next = NULL; // Initialize next pointer to NULL
	
			i += order_page_header_offset;
			
		}
		printf("BuddyAllocator::initbbbbbbbb: orders[%d] = 0x%d\n", order, get_free_blocks_len(order));
		i -= order_page_header_offset;

	}
	printf("BuddyAllocator::init: i = %d, block_info_array_size = %d\n", i, block_info_array_size);

	// printf("BuddyAllocator::init: allocated at 0x%x\n", allocate(0x100));
	// for (size_t o = 0; o < BUDDY_ORDERS; o++)
	// {
	// 	printf("BuddyAllocator::init: orders[%d] = 0x%d\n", o, get_free_blocks_len(o));
	// }
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

	if ((size_t)buddy_header_ptr < (size_t)block_info_array_start + block_info_array_size)
		// Check if the address is within the block_info_array
		return false;

	if (buddy_header_ptr->is_free())
		return false;

	uint8_t order = buddy_header_ptr->order;
	buddy_header_ptr->order = 0; // Reset the order of the block
	mark_block_unused(buddy_header_ptr, order);

	return true; // Successfully freed the block
}

void BuddyAllocator::mark_block_unused(page *page_header, uint8_t order)
{
	for (order; order < BUDDY_ORDERS; order++)
	{
		page *first;
		page *buddy;
		if (IS_ALIGNED(page_header->get_block_start(), (BUDDY_ALLOCATOR_MIN_BLOCK_SIZE << order)))
		{
			buddy = page_header + num_of_page_headers_to_next(order);
			first = page_header;
		}
		else
		{
			buddy = page_header - num_of_page_headers_to_next(order);
			first = buddy;
		}

		if (buddy->is_free())
		{
			remove_node_from_list(buddy, order);
			order++;
			page_header = first; // Update the block to the first one
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
			mark_block_used(block, order, count);
			return block->get_block_start();
		}
		count++;
	}
	return nullptr; // No suitable block found
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
	page_header->order = order; // Set the order of the block
}

void BuddyAllocator::add_node_to_list(page *page_header, uint8_t order)
{
	page *nextNode = orders[order];
	page_header->lru.next = (void *)nextNode;
	page_header->lru.prev = nullptr;
	nextNode->lru.prev = (void *)page_header;
	orders[order] = page_header;
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
			(fd)->lru.next = bk;
		if (bk != nullptr)
			fd->lru.prev = bk;
	}
	bk = nullptr;
	fd = nullptr;
}
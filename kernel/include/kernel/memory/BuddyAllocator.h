#pragma once
#include <consts.h>
#include <stdint.h>
#include <stdio.h>

#define ALIGN_UP_4K(addr) (((addr) + 0xFFF) & ~0xFFF)
#define IS_ALIGNED(addr, alignment) (((uintptr_t)(addr) & ((alignment) - 1)) == 0)
class MemoryManager; // Forward declaration

#pragma pack(push, 1) // Ensure 1-byte alignment for the BuddyBlock structure
struct list_head
{
	void *next;
	void *prev;
};
struct page
{
	uint8_t flags;    // |?|?|?|?|?|?|?|inuse|
	uint8_t order;
	struct list_head lru;

	void set_in_free_list()
	{
		
	}
	inline void set_inuse();
	inline void set_free();
	inline bool is_free();

	inline void set_chained()        { flags |= 0x02; } // 1 << 1
	inline void clear_chained()      { flags &= ~0x02; }
	inline bool is_chained() const   { return flags & 0x02; }
	
	inline void *get_block_start();
};
#pragma pack(pop) // Restore previous alignment

class BuddyAllocator
{
friend class page;
public:
	void init();

	inline size_t get_block_size_by_order(uint8_t order);
	void reserved_memory();
	inline int num_of_page_headers_to_next(size_t order)
	{
		return 1UL <<order;
	}
	inline page* get_page_header_by_address(void *address)
	{
		return (page *)block_info_array_start + ((size_t)address / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE);
	}
	int find_closest_lower_order(size_t size)
	{
		size = size >> 12;
		int order = -1;
		while (order < BUDDY_ORDERS - 1 && size > 0)
		{
			size = size >> 1;
			order++;
		}
		return order;
	}

	size_t find_closest_upper_order(size_t size)
	{
		uint8_t order = 0;
		while (order < BUDDY_ORDERS - 1 && get_block_size_by_order(order) < size)
		{
			order++;
		}
		return order;
	}
	void *allocate(size_t size);
	void *allocate(uint8_t desired_order, void *preferred_addr);

	bool free(void *ptr);

	int get_free_blocks_len(uint8_t order);

private:
	void remove_node_from_list(page *page_header, uint8_t order);
	void add_node_to_list(page *page_header, uint8_t order);

	void mark_block_used(page *page_header, uint8_t order, size_t splits);
	void mark_block_unused(page *page_header, uint8_t order);

private:
	page *orders[BUDDY_ORDERS] = {nullptr};
	void *block_info_array_start;
	size_t block_info_array_size;
};
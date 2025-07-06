#include <kernel/memory/memorymanager.h>
#include <kernel/memory/BuddyAllocator.h>


void BuddyAllocator::init(MemoryManager *memoryManager)
{
	uintptr_t buddy_structs_start = ALIGN_UP_4K(
		((size_t)(memoryManager->p_kernel_start) + memoryManager->ul_kernel_size)
	);
	uint64_t buddy_structs_size = (ALIGN_UP_4K((memoryManager->ul_physical_ram_size  / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE) * sizeof(BuddyBlock)));
	
	size_t budy_blocks_num = buddy_structs_size / BUDDY_ALLOCATOR_MIN_BLOCK_SIZE;
	printf("BuddyAllocator::init: buddy_structs_start = %x\n", buddy_structs_start);
	printf("BuddyAllocator::init: buddy_structs_size = %x\n", buddy_structs_size);
	printf("BuddyAllocator::init: budy_blocks_num = %d\n", budy_blocks_num);
}
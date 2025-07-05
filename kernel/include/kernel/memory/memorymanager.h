#pragma once
#include <cstddef>
#include <stdint.h>
#include <multiboot2.h>
uint32_t get_ram_size(int32_t mbi_addr);


class BuddyAllocator
{

};

class MemoryManager
{
	private:
    size_t ram_size;
    
public:
	BuddyAllocator buddy_allocator;
};

#pragma once
#include <cstddef>
#include <stdint.h>
#include <multiboot2.h>
#include <kernel/memory/BuddyAllocator.h>
#include <kernel/memory/PagingManager.h>


class BuddyAllocator; // Forward declaration

class MemoryManager
{
public:
	void set_physicalmemory_dimensions();
	struct multiboot_tag_elf_sections *get_elf_sections();
	void set_kernel_dimensions();

public:
	uintptr_t p_multiboot_info;

	uintptr_t ul_memory_start;
	uint64_t ul_physical_memory_size;

	uint64_t ul_kernel_size;
	uintptr_t p_kernel_start;
public:
	BuddyAllocator buddy_allocator;
	PagingManager pagingManager;

public:
	void init(uintptr_t p_multiboot_info)
	{
		this->p_multiboot_info = p_multiboot_info;
		set_physicalmemory_dimensions();
		set_kernel_dimensions();
		buddy_allocator.init();
		pagingManager.init();
		pagingManager.load();
		pagingManager.enablePaging();
	}

};

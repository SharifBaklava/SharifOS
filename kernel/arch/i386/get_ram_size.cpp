#include <kernel/memory/memorymanager.h>

#define ALIGN_UP(addr) (((addr) + 7) & ~7)


uint32_t get_ram_size(int32_t mbi_addr)
{
		uint64_t total_ram_bytes = 0;
	multiboot_tag_mmap *mmap_tag = NULL;

	// The first tag is 8 bytes after the start of the MBI structure.
	multiboot_tag *tag = (multiboot_tag *)(mbi_addr + 8);

	// Loop through all the tags
	while (tag->type != 0)
	{ // Type 0 is the end tag
		if (tag->type == 6)
		{ // Type 6 is the Memory Map tag
			mmap_tag = (multiboot_tag_mmap *)tag;
			break; // Found it, exit the loop
		}

		// Advance to the next tag. The size must be 8-byte aligned.
		tag = (multiboot_tag *)((uint8_t *)tag + ALIGN_UP(tag->size));
	}

	if (mmap_tag == NULL)
	{
		// Handle error: Memory map not found!
		// You cannot proceed with memory management.
		// print_error("Memory map not provided by bootloader!");
		return 0;
	}

	// Now, iterate through the memory map entries
	uint32_t entry_size = mmap_tag->entry_size;
	uint32_t num_entries = (mmap_tag->size - sizeof(multiboot_tag_mmap)) / entry_size;

	multiboot_mmap_entry *mmap_entry = (multiboot_mmap_entry *)((uint8_t *)mmap_tag + sizeof(multiboot_tag_mmap));

	for (uint32_t i = 0; i < num_entries; ++i)
	{
		// Check if the memory region is available RAM (type 1)
		if (mmap_entry->type == 1)
		{
			total_ram_bytes += mmap_entry->len;
		}

		// Advance to the next entry
		mmap_entry = (multiboot_mmap_entry *)((uint8_t *)mmap_entry + entry_size);
	}

	// 'total_ram_bytes' now holds the total amount of usable RAM.
	uint32_t total_ram_mb = total_ram_bytes / (1024 * 1024);
	return total_ram_mb;
}

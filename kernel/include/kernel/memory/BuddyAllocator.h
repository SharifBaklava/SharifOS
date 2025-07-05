#include <consts.h>
#include <utils/datastructures/LinkedList.h>
class BuddyAllocator {
	private:
		LinkedList<size_t> free_lists[BUDDY_ORDERS]; // Array of linked lists for each order
		size_t page_size; // Total size of the memory managed by the buddy allocator
		void* internal_data;
	public:
		BuddyAllocator(size_t total_size)
		{

		}
		size_t calc_data()
		{

		}
		void* allocate(size_t size)
		{
			size_t aligned_size = align_to2power(size);
			
		}
		void deallocate(void* ptr);
		size_t get_total_size() const;
		size_t get_free_size() const;
		size_t align_to2power(size_t size)
		{
			if (size < BUDDY_ALLOCATOR_MIN_BLOCK_SIZE) {
				return BUDDY_ALLOCATOR_MIN_BLOCK_SIZE;
			}
			size_t aligned_size = BUDDY_ALLOCATOR_MIN_BLOCK_SIZE;
			while (aligned_size < size) {
				aligned_size <<= 1; // Double the size until it is large enough
			}
			return aligned_size;
		}
};





// struct page {
//     unsigned long flags;
//     struct list_head {
//         struct page *next;
//         struct page *prev;
//     } lru;
    
//     /* ... other fields ... */
// };

// --
// -
// --
// --

// --
// --

// --



// RAM (0x0 → 0x08000000 = 128 MiB)
// 0x00100000 (1 MiB) ┌───────────────────────────┐
//                    │ Kernel ELF                │
// 0x00200000 (2 MiB) ├───────────────────────────┤
//                    │ struct free_area (~1 KB)  │ ← placed here by bootstrap allocator 11*orders head_0->page1
//                    ├───────────────────────────┤
//                    │ struct page array (2 MiB) │ ← placed here by bootstrap allocator 
// 0x00400000 (4 MiB) ├───────────────────────────┤
//                    │ Free RAM (Buddy-managed)  │
//                    │ (124 MiB free)            │
//                    └───────────────────────────┘

// struct free_area {
//     struct list_head free_list;
//     unsigned long nr_free;
// };

// struct list_head {
//     struct list_head *next;
//     struct list_head *prev;
// };
// free_area[11];

// free_page->lru.next = free_area[7]->head;
// free_area[7]->head = free_page;

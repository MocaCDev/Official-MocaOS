#ifndef MocaOS_HEAP
#define MocaOS_HEAP
#include "util.h"

// TODO:
//  Continue working on heap memory manager, perhaps add in the IDT to deal with seg faults?


#define HEAP_BLOCK_SIZE             1024
#define padding                     4

// Start address of heap allocation(depending on the block size)
static uint32 HEAP_ADDR           = 0xA000;

typedef struct HeapOutline
{
    	uint32      block_addr;
    	uint8       occupied; // 1 yes, 0 no
} HeapOutline_t;

/*
 * MocaOS_HEAP will go off of the available/unused memory blocks that we get from the
 * physical memory manager, and we will then use the bitmap representation of this
 * memory to thusly allow heap-allocations/heap-reallocations. 
 * 
 * Like any other heap allocator, we will not implement any sort of strict assignment
 * functions like we did with the physical memory manager. We will allocate the blocks,
 * give the user the address back(the address will be returned to the user via MocaHeap_t),
 * and if the user uses more than the requested size, than it's on them if they run into seg faults.
 */
static HeapOutline_t    **heap_bitmap;
static uint32           heap_index;
//static uint32   *heap_bitmap;

/*
 * MocaHeap_t:
 *  Struct containing information about heap allocation.
 *  Stores the # of blocks allocated(allocation_size), as well as the size of each block(chunk_size),
 *  and last, but not least, the allocation address(the address that points to the unused blocks within the heap_bitmap)
 */
typedef struct MocaHeap
{
    	size_t      allocation_size; // allocation size(how many blocks we want to allocate)
    	size_t      chunk_size;      // size of each block
    	uint32      *address;        // Memory address that points to available blocks within heap_bitmap.
} MocaHeap_t;

/*
 * enum Type:
 *      Type.normal = 1024 k-bytes per block
 *      Type.medium = 2048 k-bytes per block
 *      Type.large  = 4096 k-bytes per block
 */
enum Type {
	normal  = HEAP_BLOCK_SIZE,       // 1MB per block
    	medium  = HEAP_BLOCK_SIZE*2,     // 2MB per block
    	large   = HEAP_BLOCK_SIZE*3,     // 3MB per block
};
static enum Type        heap_allocation_size;

#define get_address(MOCA_HEAP)      MOCA_HEAP.address

static uint8 init_heap_bitmap(enum Type heap_allocation_type)
{
    
    	MProcess mem = m_alloc(heap_allocation_type);
    	heap_allocation_size = heap_allocation_type;

    	heap_index = 0;
    	heap_bitmap = (HeapOutline_t **)mem.address;

    	return 0;
}

// TODO:
//      Allow allocations to take up more than just 1 block at a time.
//      For now, 8192 bytes per-block seems to be plenty of heap memory.
static MocaHeap_t allocate(uint32 size, uint32 chunk_size)
{
	if((size * chunk_size) > heap_allocation_size)
	    Err((uint8 *)"Invalid allocation size: Surpassed the byte-size of a block.");

    	heap_bitmap[heap_index]->block_addr += (uint32)(HEAP_ADDR);// - allocation.address;
    	HEAP_ADDR += (size * chunk_size) * padding;
    	heap_bitmap[heap_index]->occupied   = 1;
    	heap_index++;

    	return (MocaHeap_t) {
        	.allocation_size = (uint32)size,
        	.chunk_size      = chunk_size,
        	.address         = (uint32*)heap_bitmap[heap_index-1]->block_addr
    	};
}

static void free(MocaHeap_t allocation)
{
	for(uint32 i = 0; i < heap_index; i++)
    	{
        	if(heap_bitmap[i]->block_addr == *allocation.address)
        	{
            		HEAP_ADDR -= (allocation.allocation_size * allocation.chunk_size) * padding;
            		heap_bitmap[i]->occupied = 0;

            		allocation.allocation_size = 0;
            		allocation.chunk_size = 0;
            		break;
        	}
   	}
}

#endif

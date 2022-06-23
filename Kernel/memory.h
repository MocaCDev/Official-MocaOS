#ifndef MocaOS_MEMORY
#define MocaOS_MEMORY
#include "util.h"

#define BLOCKS_PER_BYTE		8
#define BLOCK_SIZE		1024
#define BLOCK_ALIGN		BLOCK_SIZE

extern uint32 end;

static uint32 addr		= 0xA0000000;
static uint32 used_blocks	= 0;
static uint32 max_blocks	= 0;
static uint32 *memory_map	= 0;
static uint32 total_allocated 	= 0;

#define get_total_mem()		max_blocks - used_blocks

/*	Functions/Macros to manupilate memory_map	*/
void set(uint32 bit)
{
	memory_map[bit/32] |= (1 << (bit % 32));
}
void unset(uint32 bit)
{
	memory_map[bit/32] &= ~(1 << (bit % 32));
}
uint8 test(int32 bit)
{
	return memory_map[bit / 32] & (1 << (bit % 32));
}

/*	Function's/Macros to manupilate overall memory usage(allocating, freeing, reserving)	*/
uint32 kmalloc(size_t size)
{
	uint32 tmp = addr;
	addr += size;
	return tmp;
}
uint32 kmalloc_p(size_t size, int8 align)
{
	if(align == 1 && (addr & 0xFFFFF000))
	{
		addr &= 0xFFFFF000;
		addr += 0x1000;
	}
	uint32 tmp = addr;
	end += size * size;
	return tmp;
}
uint32 kmalloc_ap(size_t size, int8 align, uint32 *phys)
{
	if(align == 1 && (addr & 0xFFFFF000))
	{
		addr &= 0xFFFFF000;
		addr += 0x1000;
	}
	if(phys)
	{
		*phys = addr;
	}

	uint32 tmp = addr;
	addr += size;
	return tmp;
}

static size_t bump_allocator_base = 0x1000000;

// Only power of 2 alignments
static void *balloc_aligned(size_t count, size_t alignment) {
    size_t new_base = bump_allocator_base;
    if (new_base & (alignment - 1)) {
        new_base &= ~(alignment - 1);
        new_base += alignment;
    }
    void *ret = (void *)new_base;
    new_base += count;
    bump_allocator_base = new_base;
    return ret;
}

void *moca_alloc(size_t size)
{
	return balloc_aligned(size, 4);
}

int32 find_blocks(uint32 blocks)
{
	if(blocks == 0) return -1;

	for(uint32 i = 0; i < max_blocks / 32; i++)
	{
		if(memory_map[i] != 0xFFFFFFFF)
		{
			for(int32 j = 0; j < 32; j++)
			{
				int32 bit = 1 << j;

				if(!(memory_map[i] & bit))
				{
					int32 sb = i*32 + bit;
					uint32 free_blocks = 0;

					for(uint32 c = 0; c <= blocks; c++)
					{
						if(!test(sb + c)) free_blocks++;

						if(free_blocks == blocks)
							return i*32 + j;
					}
				}
			}
		}
	}

	return -1;
}
void init_mem(uint32 bitmap, size_t size) 
{
	memory_map = (uint32 *) bitmap;
	max_blocks = size / BLOCK_SIZE;
	used_blocks = max_blocks;
	memset(memory_map, 0xFF, max_blocks / BLOCKS_PER_BYTE);
}
void init_region(uint32 base, size_t size)
{
	int32 align	= base / BLOCK_SIZE;
	int32 blocks	= size / BLOCK_SIZE;
	for(; blocks > 0; blocks--) {
		unset(align++);
		used_blocks--;
	}
	set(0);
}
void deinit_region(uint32 base, size_t size)
{
	int32 align	= base / BLOCK_SIZE;
	int32 blocks	= size / BLOCK_SIZE;
	for(; blocks > 0; blocks--) {
		set(align++);
		used_blocks++;
	}
}

MProcess m_alloc(uint32 blocks)
{
	/* Check if we still have enough blocks for allocation. */
	if((max_blocks - used_blocks) < blocks)
	{
		Err((uint8 *)"Oversized allocation");
		return all_processes[0];
	}

	/* Get the available blocks; if fails, return a empty process. */
	int32 first = find_blocks(blocks);
	if(first == -1)
		return all_processes[0];

	/* If we obtain the blocks, set the to a status of "used". */
	for(int32 i = 0; i < blocks; i++)
		set(first+1);

	/* Get the address. If we already have a proccess(or more) going, check the allocation size of the last process, and add onto addr. */
	uint32 addr = 0;
	if(curr_pID > 0)
		addr += all_processes[curr_pID-1].allocation_size;

	/* Apply the first block and block size and add to the overall address. Then add the amount of used blocks/amount of allocated blocks. */
	addr += first * BLOCK_SIZE;
	used_blocks+=blocks;

	total_allocated += blocks;

	/* Setup the process information, increase curr_pID, return.*/
	all_processes[curr_pID].occupied	= 0;
	all_processes[curr_pID].pID		= curr_pID;
	all_processes[curr_pID].address		= addr;
	all_processes[curr_pID].allocation_size	= blocks;
	all_processes[curr_pID].allocation_used = 0;
	curr_pID++;

	return all_processes[curr_pID-1];
}

void m_free(MProcess *process)
{
	/* Get frame, and unset memory regions. Subtract allocation size from used blocks. */
	int32 frame = process->address / BLOCK_SIZE;
	for(uint32 i = 0; i < process->allocation_size; i++)
	{
		unset(frame+i);
	}
	used_blocks -= process->allocation_size;
	
	/* Make sure to clear out all buffer in the memory address. */
	uint32 *addr = (uint32 *)process->address;
	for(uint32 i = 0; i < process->allocation_size; i++)
		*addr++ = '\0';
	
	/* Clear out process information to default. */
	total_allocated -= process->allocation_size;
	process->occupied	= 1;
	process->pID		= 0;
	process->address	= 0;
	process->allocation_size= 0;
	process->allocation_used= 0;
	
	/* Update process all together. Subtract the total amount of processes. */
	all_processes[process->pID] = *process;
	curr_pID--;
}

/*	Functions/Macros to assign/use memory allocations.	*/
void assign_value(MProcess *process, uint8 value)
{
	/* Check if the allocation size has be occupied */
	if(process->allocation_used == process->allocation_size)
		return;

	/* If we dont return, we still have room. Lets check the amount of room we have. */
	if(process->allocation_used + 1 >= process->allocation_size)
		return;

	/* No? Okay, lets assign the byte value to the next open address. */
	uint32 *addr = (uint32 *)process->address;
	addr[process->allocation_used] = value;
	process->allocation_used++;

	/* Update processes. */
	all_processes[process->pID] = *process;
}

#endif

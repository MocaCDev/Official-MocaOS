#ifndef MocaOS_MEMORY
#define MocaOS_MEMORY
#include "util.h"

#define BLOCKS_PER_BYTE		8
#define BLOCK_SIZE		4096
#define BLOCK_ALIGN		BLOCK_SIZE

extern uint32 end;
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

/*	Functions/Macros to manupilate overall memory usage(allocating, freeing, reserving)	*/
uint32 kmalloc(size_t size, size_t alloc_size)
{
	uint32 tmp = (uint32)&end;
	end += size * alloc_size;
	return tmp;
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
	if((max_blocks - used_blocks) < blocks)
	{
		Err((uint8 *)"Oversized allocation");
		return all_processes[0];
	}

	int32 first = find_blocks(blocks);

	if(first == -1)
		return all_processes[0];

	for(int32 i = 0; i < blocks; i++)
		set(first+1);


	uint32 addr = 0;

	if(curr_pID > 0)
		addr += all_processes[curr_pID-1].allocation_size;

	addr += first * BLOCK_SIZE;

	used_blocks+=blocks;

	total_allocated += blocks;

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
	int32 frame = process->address / BLOCK_SIZE;

	for(uint32 i = 0; i < process->allocation_size; i++)
	{
		unset(frame+i);
	}
	used_blocks -= process->allocation_size;
	
	// Make sure to clear out all buffer in the memory address
	uint32 *addr = (uint32 *)process->address;
	for(uint32 i = 0; i < process->allocation_size; i++)
		*addr++ = '\0';
	
	total_allocated -= process->allocation_size;
	process->occupied	= 1;
	process->pID		= 0;
	process->address	= 0;
	process->allocation_size= 0;
	process->allocation_used= 0;

	all_processes[process->pID] = *process;
	curr_pID--;
}

/*	Functions/Macros to assign/use memory allocations.	*/
void assign_value(MProcess *process, uint8 value)
{
	// Check if the allocation size has be occupied
	if(process->allocation_used == process->allocation_size)
		return;

	uint32 *addr = (uint32 *)process->address;
	addr[process->allocation_used] = value;
	process->allocation_used++;

	all_processes[process->pID] = *process;
}

#endif

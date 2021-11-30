#ifndef MocaOS_MEMORY
#define MocaOS_MEMORY
#include "util.h"

#define BLOCKS_PER_BYTE		8
#define BLOCK_SIZE		4096
#define BLOCK_ALIGN		BLOCK_SIZE

static uint32 used_blocks	= 0;
static uint32 max_blocks	= 0;
static uint32 *memory_map	= 0;

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

#endif

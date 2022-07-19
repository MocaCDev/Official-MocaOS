#ifndef MocaPMM
#define MocaPMM
#include "util.h"

typedef struct MocaPMM 
{
	uint32 PMM_start;
	uint32 *bitmap;
	uint32 max_blocks;
	uint32 used_blocks;
	uint32 free_blocks;
} _MocaPMM;

typedef struct AllocInfo
{
	uint32		RAW_addr;
	uint32		PTR_addr;
	uint32		size;
} _AllocInfo;

static _MocaPMM moca_pmm;

#define BLOCK_SIZE		1024
#define BPB				8		// Blocks Per Byte
#define BIT(b)		(1 << (b % 32))
#define CHECK_ENTRY(b)	\
		bitmap[b/32] & BIT(b) ? 0 : 1

void set_entry(uint32 bit) 
{
	moca_pmm.bitmap[b/32] |= BIT(bit);
}

void unset_entry(uint32 bit)
{
	moca_pmm.bitmap[b/32] &= ~(BIT(bit));
}

void init_pmm(uint32 start_addr, uint32 total_entries)
{
	moca_pmm.PMM_start = start_addr;
	moca_pmm.bitmap = (uint32 *) start_addr;
	moca_pmm.max_blocks = total_entries / BLOCK_SIZE;
	moca_pmm.used_blocks = moca_pmm.max_blocks;
	moca_pmm.free_blocks = 0;
	memset(moca_pmm.bitmap, 0xFF, moca_pmm.max_blocks);
}

void init_region(uint32 addr, uint32 length)
{
	uint32 align = addr / BLOCK_SIZE;
	uint32 blocks = length / BLOCK_SIZE;

	for(uint32 i = blocks; i > 0; i--)
	{
		unset_entry(align++);
		moca_pmm.used_blocks--;
		moca_pmm.free_blocks++;
	}
}

void deinit_region(uint32 addr, uint32 length)
{
	uint32 align = addr / BLOCK_SIZE;
	uint32 blocks = length / BLOCK_SIZE;

	for(uint32 i = blocks; i > 0; i--)
	{
		set_entry(align++);
		moca_pmm.used_blocks++;
		moca_pmm.free_blocks--;
	}
}

#endif
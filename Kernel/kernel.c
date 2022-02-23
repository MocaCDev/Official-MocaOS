#include "mstdio.h"
#include "memory.h"
#include "terminal.h"
#include "idt.h"

#define MEMADDR	0x15000

__attribute__((section("kernel_entry"))) void kernel_main(void)
{

	_Smap *smap = (_Smap *)0x8504;
	uint32 entries = *(uint32 *)0x8500;
	smap += entries - 1;
	uint32 total = smap->base + smap->length - 1;

	init_mem(MEMADDR, total);

	smap = (_Smap *)0x8504;
	for(uint32 i = 0; i < entries; i++)
	{
		if(smap->type == 1)
			init_region(smap->base, smap->length);
		smap++;
	}
	
	/*
	 * From 0x1000 to the end of the kernel(0x2800), reserve.
	 * Everything beyond 0x2800 is free grabs.
	 * */
	deinit_region(0x1F00, (uint32)&end);
	deinit_region(MEMADDR, max_blocks / BLOCKS_PER_BYTE);	

	*L_OS_INFO_ADDR = OS_IN_ACTION;
	MOVE_ADDR(2);

	terminal();
}

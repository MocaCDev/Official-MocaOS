#include "mstdio.h"
#include "memory.h"
#include "terminal.h"
#include "idt.h"

#define MEMADDR	0x15000

__attribute__((section("kernel_entry"))) void kernel_main(void)
{

	_Smap *smap = (_Smap *)0x8504;
	uint32 entries = *(uint32 *)0x8500;
	
	/* Get total amount of available memory from all regions. */
	uint32 total = 0;
	for(uint32 i = 0; i < entries; i++)
	{
		switch(smap->type)
		{
			case 1: total += smap->length; break;
			default: break;
		}
		smap++;
	}

	/* Initialize physical memory(should be 127MB of available memory, with 1024 byte block sizes). */
	init_mem(MEMADDR, total);

	/* Inititalize each region(even the region of the kernel) to map out all available memory location. Deinit kernel locations afterwards. */
	smap = (_Smap *)0x8504;
	for(uint32 i = 0; i < entries; i++)
	{
		if(smap->type == 1)
			init_region(smap->base, smap->length);
		smap++;
	}
	
	/* TODO: Remove? */
	/* Deinit all memory regions between start of kernel and end; preserve the memory of the kernel, do not override. */
	deinit_region(0x1F00, (uint32)&end);

	/* MocaOS is now in full action. */
	*L_OS_INFO_ADDR = OS_IN_ACTION;
	MOVE_ADDR(2);

	terminal();
}

#include "mstdio.h"
#include "memory.h"

typedef struct Smap
{
	uint64	base;
	uint64	length;
	uint32	type;
	uint32	acpi;
} __attribute__((packed)) _Smap;

void print_mem_info(void);

__attribute__((section("kernel_entry"))) void kernel_main(void)
{
	_Smap *smap = (_Smap *)0x8504;
	uint32 entries = *(uint32 *)0x8500;
	smap += entries - 1;
	uint32 total = smap->base + smap->length - 1;

	init_mem(0x15000, total);

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
	deinit_region(0x1000, (uint32)&end);
	deinit_region(0x15000, max_blocks / BLOCKS_PER_BYTE);
	Print((uint8 *)"Memory Initialized\n");

	while(1);
}

void print_mem_info()
{
	uint32 entries = *(uint32 *)0x8500;
	_Smap *smap = (_Smap *)0x8504;

	Print((uint8 *)
		"TYPE\t\tREGION\tBASE (LENGTH)\n"
		"=========   ======\t=============\n"
	     );

	for(uint32 i = 0; i < entries; i++)
	{
		switch(smap->type)
		{
			case 1:
				Print((uint8 *)"(AVAILABLE) ");
				break;
			case 2:
				Print((uint8 *)"(RESERVED)  ");
				break;
			case 3:
				Print((uint8 *)"(ACPI Reclaimed) ");
				break;
			case 4:
				Print((uint8 *)"(ACPI NVS Memory) ");
				break;
			default:
				Print((uint8 *)"(RESERVED)  ");
				break;
		}

		PrintHex(i);
		for(uint8 i = 0; i < 7; i++)
			PutC(' ');

		PrintHex(smap->base);

		Print((uint8 *)" (");
		PrintHex(smap->length);
		Print((uint8 *)")");

		Print((uint8 *)"\n");

		smap++;
	}

	smap--;
	Print((uint8 *)"Total: ");
	PrintHex(smap->base + smap->length - 1);
	Print((uint8 *)", Used: ");
	PrintHex((uint32)used_blocks);
	Print((uint8 *)", Available: ");
	PrintHex(max_blocks-used_blocks);
	Print((uint8 *)", Allocated: ");
	PrintHex(total_allocated);
}

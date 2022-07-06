#include "mstdio.h"
#include "memory.h"
#include "terminal.h"
#include "mouse.h"
#include "idt.h"
//#include "virtual_mem.h"

#define MEMADDR	0xFFF80000

extern void loadPageDir(uint32 *);
extern void enablePaging();

__attribute__((section("kernel_entry"))) void kernel_main(void)
{
	*L_OS_INFO_ADDR = INITIAL_STATE;
	//L_OS_INFO_ADDR += 2;
	pic_remap(0x20, 0x28);
	idt_init();
	pic_set_mask(2, 0);
	init_pit();
	pic_set_mask(0, 0);
	init_mouse();
	pic_set_mask(12, 0);
	//uint32 i = 0;
	//while(i <= 50)
	//	put_pixel(1, i, GREEN);	
	//remap_pic(0x20, 0x28);
    //idt_init();

	//__asm__ __volatile__("movl $0x1D, %edx\nmovl $0, %eax\nint $0x80");

	/*__asm__ __volatile__(
		"movl $0, %eax\n"
		"movl $0, %ebx\n"
		"xor %edx, %edx\n"
		"div %ebx"
	);*/

    // enable cascade
    //pic_set_mask(2, 0);

    //init_pit();
    //pic_set_mask(0, 0);

    //init_mouse();
    //pic_set_mask(12, 0);
	PrintHex((uint32) MEMADDR);

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
	init_pmm(MEMADDR, total);
	//Print((uint8 *)"Hey", WHITE, BLACK);

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
	//*L_OS_INFO_ADDR = OS_IN_ACTION;
	//MOVE_ADDR(2);

	//idt_init();
	//init_pit();

	//init_mouse();
	//init_windows();

	//window_create(
	//	"test", 30, 30,
	//	800, 400
	//);
	//refresh();

	//while(1);

	__asm__ __volatile__("sti");
	//while(1);
	terminal();
}

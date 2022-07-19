#include "mstdio.h"
#include "memory.h"
#include "terminal.h"
#include "mouse.h"
#include "idt.h"
//#include "virtual_mem.h"

#define MEMADDR	0xFFF80000
#define BMP_HDR_ADDR 0xD000
#define BMP_PXL_ADDR (BMP_HDR_ADDR + 18)

extern void loadPageDir(uint32 *);
extern void enablePaging();

typedef struct HeaderInfo {
	uint8	id[6]; 				// "MOCAF"
	uint32	amnt_per_scanline;	// Amnt of pixels per row
	uint32	length;				// Amnt of rows
	uint32	total;				// Total bytes from image
} __attribute__((packed)) Header_Info;

__attribute__((section("kernel_entry"))) void kernel_main(void)
{
	uint8 *addr = (uint8 *)BMP_HDR_ADDR;
	uint32 *fb = (uint32 *)FB;

	tc.cursor_x = 25;
	tc.cursor_y = 25;

	addr += 6;
	PrintHex((uint32) *addr);

	addr += 4;

	PrintHex((uint32) *addr);

	addr+=4;
	PrintHex((uint32) *addr);

	__asm__ ("cli; hlt");

	Header_Info *i = (Header_Info *) addr;
	//Print((uint8 *) i->a1, WHITE, BLACK);
	//PrintHex((uint32) i->a2);

	Print((uint8 *) i->id, WHITE, BLACK);
	Print((uint8 *)"\n", BLACK, BLACK);
	PrintHex((uint32) i->amnt_per_scanline);
	Print((uint8 *)"\n", BLACK, BLACK);
	PrintHex((uint32) i->length);
	Print((uint8 *)"\n", BLACK, BLACK);
	PrintHex((uint32) i->total);
	Print((uint8 *)"\n", BLACK, BLACK);
	PrintHex((uint32) *(uint32 *)BMP_PXL_ADDR);

	__asm__ ("cli; hlt");


	//addr++;
	
	uint8 amnt_per_scanline = (uint8 )*addr;
	uint8 current_amnt = 0;
	addr++;
	uint8 current_width = (uint8)*addr++;
	/*Print((uint8 *)"Amnt Per Scanline: ", WHITE, BLACK);
	PrintHex((uint32) amnt_per_scanline);
	Print((uint8 *)"\n", BLACK, BLACK);
	Print((uint8 *)"Current Width: ", WHITE, BLACK);
	PrintHex((uint32) current_width);*/

	tc.cursor_x = 5;
	tc.cursor_y = 10;
	//PrintHex((uint32) RED);

	fb += (tc.cursor_x * 8) + (tc.cursor_y * 16 * WIDTH);
	for(uint8 i = 0; ; i++)
	{
		if(*(uint32 *)addr == 257)
		{
			addr++;
			if(*(uint8 *)addr == 'E') break;
			current_width = (uint8) *addr;
			//Print((uint8 *)"Current Width: ", WHITE, BLACK);
			//PrintHex((uint32) current_width);
			addr++;
		}
		for(int8 b = current_width; b >= 0; b--)
		{
			*fb = *addr;
			fb++;
		}
		addr++;
		if(!(*(uint32 *)addr == 257))
		{
			fb+=(WIDTH - (current_amnt+1));
			current_amnt++;
		} else
			current_amnt = 0;
	}
	__asm__ ("cli; hlt");
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

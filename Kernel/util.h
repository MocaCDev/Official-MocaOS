#ifndef MocaOS_UTIL
#define MocaOS_UTIL

typedef unsigned char		uint8;
typedef char			int8;
typedef unsigned short		uint16;
typedef short			int16;
typedef unsigned int		uint32;
typedef int			int32;
typedef unsigned long long	uint64, size_t;
typedef long long		int64;

/*	INFO STRUCTS	*/
// VesaInfoBlock:
// 	Information about the current vesa memory mode.
typedef struct VesaInfoBlock
{
	uint16		attributes;
	uint8		window_a;
	uint8		window_b;
	uint16		gran;
	uint16		window_size;
	uint16		seg_a;
	uint16		seg_b;
	uint32		win_func_ptr;
	uint16		pitch;
	uint16		width;
	uint16		height;
	uint8		w_char;
	uint8		y_char;
	uint8		planes;
	uint8		bpp;
	uint8		banks;
	uint8		memory_model;
	uint8		bank_size;
	uint8		image_pages;
	uint8		reserved1;
	uint8		red_mask;
	uint8		red_pos;
	uint8		green_mask;
	uint8		green_pos;
	uint8		blue_mask;
	uint8		blue_pos;
	uint8		reserved_mask;
	uint8		reserved_pos;
	uint8		direct_color_attributes;
	uint32		framebuffer;
	uint32		off_screen_mem_off;
	uint16		off_screen_mem_size;
	uint8		reserved2[206];
} __attribute__((packed)) Vesa_Info_Block;

// TerminalCursor
//	Information about current position.
typedef struct TerminalCursor
{
	uint16	cursor_x;
	uint16	cursor_y;
} Terminal_Cursor;

// MemProcess
// 	All processes the kernel is handling.
// 	Keep track of overall memory between each allocation.
static uint32 curr_pID = 0;
typedef struct MemProcess
{
	uint8	occupied;
	uint32	pID;
	uint32	address;
	size_t	allocation_size;
	size_t	allocation_used;
} MProcess;

typedef struct Smap
{
	uint64	base;
	uint64	length;
	uint32	type;
	uint32	acpi;
} __attribute__((packed)) _Smap;

/*
 * Allow only 25 processes at once.
 * Once there is 25 processes going, you'll have to end another process to start a new one.
 * */
static MProcess all_processes[25];

static Terminal_Cursor tc = {
	.cursor_x = 0,
	.cursor_y = 0
};

Vesa_Info_Block *VIB = (Vesa_Info_Block*)0x4000;
static uint32 ticks = 0x0;
#define FREQUENCY		1000

/*	OS specific addresses.		*/
extern uint32		end;
#define PLACEHOLDER	(uint32)&end // Used for nothing; placeholder for address space of OS_INFO

/*
 * OS_INFO:
 * 	If there is a OS_LOCKED "flag" placed within this address, that means there was an underlying
 * 	problem that the OS picked up, instead of the IDT. If there is any flag set in this
 * 	address, other than the OS_IN_ACTION "flag",
 * 	the OS will begin to take action. Eventually, the OS_LOCKED flag will be set,
 * 	and the OS will self-reboot(soft reboot), to hopefully fix the problem.
 * 	The OS_IN_ACTION flag should be set at all times until problems arise.
 *
 * 	OS_INFO stores all information of the OS process:
 * 		 1B  2B
 * 		(--)(--) : Each value takes up 2 bytes. The first 2 bytes is the OS status.
 *
 * 		(--)(--),(--)(--),(--)(--),
 * 		   1st      2nd     3rd
 * 			The above sequence is the outline of the information being stored
 * 			in OS_INFO
 * 		1st - The OS status(OS_IN_ACTION, OVERDUE_HEAP, OS_LOCKED)
 * 		2nd - The status of the IDT(0x0 or 0x1B01 - INTERRUPTS_ENABLED)
 * 		3rd - Heap information. This may contain more than just a simple "flag".
 * 		      At this point of the address, it might point to a different address where
 * 		      we can reference all memory.
 */
#define OS_INFO		(uint32)PLACEHOLDER // OS_INFO placed at placeholder address

/*	Static variables for last address of OS specific address spaces.	*/
static uint32 *L_OS_INFO_ADDR  = (uint32 *)OS_INFO;
static uint32 *L_PLACEHOLDER   = (uint32 *)PLACEHOLDER;

/*	OS specific hex values. 	*/
/*	1B specific hex values.
 *	
 *	1B specific hex values are for the common areas of the OS,
 *	such as the memory, the IDT, the heap etc.
 *	 	
 */
#define EMPTY					0x00A1
#define INITIAL_STATE			0x1A00
#define INTERRUPTS_ENABLED		0x1B00
#define MEMORY_ENABLED			0x1B01
#define HEAP_ENABLED			0x1B02

/*	2B specific hex values.
 *
 * 	2B specific hex values are for OS specific flags which help decipher errors/problems
 * 	within the OS. The "always on" flag, or the OS_IN_ACTION flag, should always be set.
 * 	If the OS_IN_ACTION flag is not set, then there must be an error.
 */
#define OS_IN_ACTION			0x2B00
#define OVERDUE_HEAP			0x2B01
#define OS_LOCKED			0x2B02

/*	Common constants	*/
#define FB		VIB->framebuffer // Vesa framebuffer memory address used to place pixels
#define pitch	VIB->pitch
#define width	VIB->width
#define height	VIB->height
#define FONT	0x9000		 // Custom OS font memory location
#define WIDTH	1024		 // Width of terminal
#define HEIGHT	768		 // Height of terminal
#define MakeColor(R, G, B) (uint32)(R*65536 + G*256 + B	)// Equation to get RGB hex value
#define resize(size)	size % 2 // Get remainder value					 

/*	GENERAL COLORS	*/
#define	WHITE	MakeColor(255, 255, 255)
#define BLACK	MakeColor(  0,   0,   0)
#define RED	MakeColor(255,	 0,   0)
#define GREEN	MakeColor(  0, 255,   0)
#define BLUE	MakeColor(  0,   0, 255)

/*	General functions.	*/

#define abs(number) ((number < 0) ? (-number) : (number))

void *memset(void *buf, uint8 byte, uint32 length)
{
	uint8 *val = (uint8 *)buf;
	for(uint32 i = 0; i < length; i++)
		val[i] = byte;
	return buf;
}

uint8 PORT_IN(uint16 port)
{
	uint8 rv;
	__asm__ __volatile__ ("in %1, %0" : "=a"(rv) : "dN"(port));
	return rv;
}
void PORT_OUT(uint16 port, uint8 data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN"(port), "a"(data));
}

unsigned read_pit_count(void) {
	unsigned count = 0;
 
	// Disable interrupts
	//cli();
	__asm__ __volatile__ ("cli;");
 
	// al = channel in bits 6 and 7, remaining bits clear
	PORT_OUT(0x43,0b0000000);
 
	count = PORT_IN(0x40);		// Low byte
	count |= PORT_IN(0x40)<<8;		// High byte
 
	return count;
}

void update_screen()
{
	uint32 *ss, *ss2;

	if(tc.cursor_y >= 46)
	{
		ss = (uint32 *) FB;
		ss2 = ss + (1024 * 16);

		for(uint32 p = 0; p < 0xBA000; p++)
			*ss++ = *ss2++;
		for(uint32 p = 0; p < 0x6000; p++)
			*ss++ = BLACK;
		tc.cursor_x = 0;
		tc.cursor_y--;
		return;
	}
}

void clear_screen()
{
	uint32 *fb = (uint32 *) FB;

	for(uint32 i = 0; i < WIDTH * HEIGHT; i++)
		fb[i] = BLACK;

	tc.cursor_x = 0;
	tc.cursor_y = 0;
}

#define MOVE_ADDR(b) \
	L_OS_INFO_ADDR += b; \
	L_PLACEHOLDER  += b

#endif

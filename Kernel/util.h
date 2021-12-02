#ifndef MocaOS_UTIL
#define MocaOS_UTIL

typedef unsigned char		uint8;
typedef char			int8;
typedef unsigned short		uint16;
typedef short			int16;
typedef unsigned int		uint32;
typedef int			int32;
typedef unsigned long long	uint64;
typedef long long		int64;
typedef uint64			size_t;

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

#define FB	VIB->framebuffer			 // Vesa framebuffer memory address used to place pixels
#define FONT	0x1000					 // Custom OS font memory location
#define WIDTH	800					 // Width of terminal
#define HEIGHT	600					 // Height of terminal
#define MakeColor(R, G, B) (uint32)(R*65536 + G*256 + B	)// Equation to get RGB hex value

/*	GENERAL COLORS	*/
#define	WHITE	MakeColor(255, 255, 255)
#define BLACK	MakeColor(  0,   0,   0)

/*	General functions.	*/
void *memset(void *buf, uint8 byte, uint32 length)
{
	uint8 *val = (uint8 *)buf;
	for(uint32 i = 0; i < length; i++)
		val[i] = byte;
	return buf;
}

uint8 inb(uint16 port)
{
	uint8 rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a"(rv) : "dN"(port));
	return rv;
}
void outb(uint16 port, uint8 data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN"(port), "a"(data));
}

#endif

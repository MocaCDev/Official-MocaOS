#ifndef MocaSTDIO
#define MocaSTDIO
#include "util.h"

enum kb_scancodes {
	escape		= 0x1,
	SC_1		= 0x2,
	SC_2		= 0x3,
	SC_3		= 0x4,
	SC_4		= 0x5,
	SC_5		= 0x6,
	SC_6		= 0x7,
	SC_7		= 0x8,
	SC_8		= 0x9,
	SC_9		= 0xA,
	SC_0		= 0xB,
	minus		= 0xC,
	equals		= 0xD,
	backspace	= 0xE,
	tab		= 0xF,
	q		= 0x10,
	w		= 0x11,
	e		= 0x12,
	r		= 0x13,
	t		= 0x14,
	y		= 0x15,
	u		= 0x16,
	i		= 0x17,
	o		= 0x18,
	p		= 0x19,
	a		= 0x1E,
	s		= 0x1F,
	d		= 0x20,
	f		= 0x21,
	g		= 0x22,
	h		= 0x23,
	j		= 0x24,
	k		= 0x25,
	l		= 0x26,
	z		= 0x2C,
	x		= 0x2D,
	c		= 0x2E,
	v		= 0x2F,
	b		= 0x30,
	n		= 0x31,
	m		= 0x32,
	space		= 0x39
};

typedef struct scancodes
{
	uint8 val;

	union {
		struct {
			enum kb_scancodes sc;
		};
	};
} _scancodes;

static const _scancodes _vals[] = {
	[escape]	= {.val = '\0'},
	[SC_1]		= {.val = '1'},
	[SC_2]		= {.val = '2'},
	[SC_3]		= {.val = '3'},
	[SC_4]		= {.val = '4'},
	[SC_5]		= {.val = '5'},
	[SC_6]		= {.val = '6'},
	[SC_7]		= {.val = '7'},
	[SC_8]		= {.val = '8'},
	[SC_9]		= {.val = '9'},
	[SC_0]		= {.val = '0'},
	[minus]		= {.val = '-'},
	[equals]	= {.val = '='},
	[backspace]	= {.val = '\b'},
	[tab]		= {.val = '\t'},
	[q]		= {.val = 'q'},
	[w]		= {.val = 'w'},
	[e]		= {.val = 'e'},
	[r]		= {.val = 'r'},
	[t]		= {.val = 't'},
	[y]		= {.val = 'y'},
	[u]		= {.val = 'u'},
	[i]		= {.val = 'i'},
	[o]		= {.val = 'o'},
	[p]		= {.val = 'p'},
	[a]		= {.val = 'a'},
	[s]		= {.val = 's'},
	[d]		= {.val = 'd'},
	[f]		= {.val = 'f'},
	[g]		= {.val = 'g'},
	[h]		= {.val = 'h'},
	[j]		= {.val = 'j'},
	[k]		= {.val = 'k'},
	[l]		= {.val = 'l'},
	[z]		= {.val = 'z'},
	[x]		= {.val = 'x'},
	[c]		= {.val = 'c'},
	[v]		= {.val = 'v'},
	[b]		= {.val = 'b'},
	[n]		= {.val = 'n'},
	[m]		= {.val = 'm'},
	[space] 	= {.val = ' '}
};


void PutC(uint8 c, uint32 color1, uint32 color2)
{
	uint8 *val = (uint8 *)(FONT + ((c * 16) - 16));
	uint32 *fb = (uint32 *)FB;

	fb += (tc.cursor_x * 8) + (tc.cursor_y * 16 * WIDTH);

	for(uint8 i = 0; i < 16; i++)
	{
		for(int8 b = 7; b >= 0; b--)
		{
			*fb = (val[i] & (1 << b)) ? color1 : color2;
			fb++;
		}
		fb += (WIDTH - 8);
	}

	tc.cursor_x++;
}

void Print(uint8 *str, uint32 fg, uint32 bg)
{
	while(*str != 0)
	{
		switch(*str)
		{
			case '\n':
			{
				tc.cursor_y++;
				tc.cursor_x = 0;
				str++;
				update_screen();
				break;
			}
			case '\t':
			{
				tc.cursor_x += 4;
				str++;
				break;
			}
			default:
			{
				PutC(*str, fg, bg);
				str++;
				break;
			}
		}
	}
}

void Err(uint8 *str)
{
	Print(str, MakeColor(255, 0, 0), BLACK);

	//__asm("hlt");
}

#define PANIC(msg) \
	Err((uint8 *)msg); \
	__asm("hlt;");

void PrintHex(uint32 number)
{

	if(number <= 0)
	{
		uint8 *val = (uint8 *)"0x0";
		Print((uint8 *)val, WHITE, BLACK);
		return;
	}

	uint8 hex[80];
	uint8 numbers[17] = "0123456789ABCDEF";
	uint8 i = 0, j = 0;

	while(number > 0)
	{
		hex[i] = numbers[(uint8)number & 0x0F];
		number >>= 4;
		i++;
	}


	hex[i++] = 'x';
	hex[i++] = '0';
	hex[i] = '\0';
	i--;

	for(j=0; j < i; j++, i--)
	{
		uint8 temp = hex[j];
		hex[j] = hex[i];
		hex[i] = temp;
	}
	Print((uint8 *)hex, WHITE, BLACK);
}

void PrintNum(uint32 _number)
{
	uint8 dec[80];
	uint8 i = 0, j, temp;

	while(_number > 0)
	{
		dec[i] = (_number % 10) + '0';
		_number /= 10;
		i++;
	}

	dec[i] = '\0';
	i--;

	for(j = 0; j < i; j++, i--)
	{
		temp = dec[j];
		dec[j] = dec[i];
		dec[i] = temp;
	}

	Print((uint8 *)dec, WHITE, BLACK);
}

uint8 get_key()
{
	uint8 scancode	= 0;
	uint8 char_val 	= 0;
	uint8 tb 	= 0;
	uint8 *shifts   = (uint8 *)")!@#$%^&*(";

	while(1)
	{
		__asm__ __volatile__ ("inb $0x64, %%al" : "=a"(tb));
		if(tb & 1) break;
	}

	__asm__ __volatile__("inb $0x60, %%al" : "=a"(scancode));

	if(scancode == 0x1C)
		return 0x1C;
	if(scancode == 0x2A)
	{	
		while(1)
		{
			__asm__ __volatile__ ("inb $0x64, %%al" : "=a"(tb));
			if(tb & 1) break;
		}

		__asm__ __volatile__("inb $0x60, %%al" : "=a"(scancode));

		char_val = (uint8)_vals[scancode].val;

		if(char_val >= '0' && char_val <= '9')
			return shifts[char_val-0x30];
			

		return (uint8)_vals[scancode].val - 0x20;
	}

	return (uint8)_vals[scancode].val;
}

/*void get_mouse()
{
	outb(0xD4, 0x64);
	outb(0xF3, 0x60);

	while(!(inb(0x64) & 1)) __asm("pause");
	uint32 ack = inb(0x60);
	PrintHex(ack);
	outb(0xD4, 0x64);
	outb(100, 0x60);
	while(!(inb(0x64) & 1)) __asm("pause");
	ack = inb(0x60);
	PrintHex(ack);
}*/

#endif

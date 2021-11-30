#ifndef MocaSTDIO
#define MocaSTDIO
#include "util.h"

void PutC(uint8 c)
{
	uint8 *val = (uint8 *)(FONT + ((c * 16) - 16));
	uint32 *fb = (uint32 *)FB;

	fb += (tc.cursor_x * 8) + (tc.cursor_y * 16 * WIDTH);

	for(uint8 i = 0; i < 16; i++)
	{
		for(int8 b = 7; b >= 0; b--)
		{
			*fb = (val[i] & (1 << b)) ? WHITE : BLACK;
			fb++;
		}
		fb += (WIDTH - 8);
	}

	tc.cursor_x++;
}

void Print(uint8 *str)
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
				PutC(*str);
				str++;
				break;
			}
		}
	}
}

void PrintHex(uint32 number)
{

	if(number <= 0)
	{
		uint8 *val = (uint8 *)"0x0";
		Print((uint8 *)val);
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
	Print((uint8 *)hex);

}

#endif

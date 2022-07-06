#ifndef TERM
#define TERM
#include "string.h"
#include "idt.h"

void print_mem_info()
{	
	uint32 entries = *(uint32 *)0x8500;
	_Smap *smap = (_Smap *)0x8504;

	Print((uint8 *)
		"\n\nTYPE\t\tREGION\tBASE (LENGTH)\n"
		"=========   ======\t=============\n",
	    WHITE, BLACK);

	for(uint32 i = 0; i < entries; i++)
	{
		switch(smap->type)
		{

			case 1:
				Print((uint8 *)"(AVAILABLE) ", WHITE, BLACK);
				break;
			case 2:
				Print((uint8 *)"(RESERVED)  ", WHITE, BLACK);
				break;
			case 3:
				Print((uint8 *)"(ACPI Reclaimed) ", WHITE, BLACK);
				break;
			case 4:
				Print((uint8 *)"(ACPI NVS Memory) ", WHITE, BLACK);
				break;
			default:
				Print((uint8 *)"(RESERVED)  ", WHITE, BLACK);
				break;
		}

		PrintHex(i);
		for(uint8 i = 0; i < 7; i++)
			PutC(' ', WHITE, BLACK);

		PrintHex(smap->base);

		Print((uint8 *)" (", WHITE, BLACK);
		PrintHex(smap->length);
		Print((uint8 *)")", WHITE, BLACK);

		Print((uint8 *)"\n", WHITE, BLACK);

		smap++;
	}

	smap--;
	Print((uint8 *)"Total: ", WHITE, BLACK);
	PrintHex(smap->base + smap->length - 1);
	Print((uint8 *)", 4kb blocks: ", WHITE, BLACK);
	PrintHex((uint32)moca_pmm.max_blocks);
	Print((uint8 *)", Used: ", WHITE, BLACK);
	PrintHex((uint32)moca_pmm.used_blocks);
	Print((uint8 *)", Available: ", WHITE, BLACK);
	PrintHex(moca_pmm.max_blocks-moca_pmm.used_blocks);
	Print((uint8 *)"(", WHITE, BLACK);
	PrintNum((moca_pmm.max_blocks-moca_pmm.used_blocks)/1024);
	Print((uint8 *)"MB)", WHITE, BLACK);
	Print((uint8 *)", Allocated: ", WHITE, BLACK);
	//PrintHex(total_allocated);
	Print((uint8 *)"\n", BLACK, BLACK);
}

void terminal()
{
	//get_mouse();
	Print((uint8 *)"Welcome to MocaOS!\n", WHITE, BLACK);
	Print((uint8 *)"\tTo Get Started:\n", WHITE, BLACK);
	Print((uint8 *)"\t\t * init heap TYPE(Where TYPE is Normal, Medium or Large) \n\t\t * init sudo user(allows super access to terminal functionality) \n\t\t * init settings(go through all terminal settings, eg. Terminal color, Terminal cursor etc) \n", BLACK, MakeColor(119, 255, 91));
	Print((uint8 *)"\t\t * OR type init to see all available choices\n\n", WHITE, BLACK);
	
	PutC((uint8)'>', WHITE, BLACK);

	uint8 input;
	uint8 string[25] = {0};
	uint32 length = 0;

	while(1)
	{
		input = get_key();

		if(input == 0x48)
		{
			Print((uint8 *)"Up", RED, BLACK);
			__asm("cli;hlt");
		}
		if(!(input == 0x1C) && (input >= 'a' && input <= 'z'))
		{
			PutC((uint8)input, WHITE, BLACK);
			string[length] = input;
			length++;

			continue;
		}
		if(input == 0x1C)
		{
			if(strcmp((uint8 *)string, (uint8 *)"mem")==0)
				print_mem_info();
			else if(strcmp((uint8 *)string, (uint8 *)"status")==0)
			{
				//PrintHex((uint32) *((uint32 *)OS_INFO));
				uint32 *temp = (uint32 *)OS_INFO;
				for(;;) {
					if(*((uint32 *)temp) == EMPTY)
						goto end;
					switch(*((uint32 *)temp)) {
						case INITIAL_STATE: Print((uint8 *)"\n\n\tInitial State", WHITE, BLACK); break;
						case INTERRUPTS_ENABLED: Print((uint8 *)"\n\n\tEnterrupts Enabled.", WHITE, BLACK); break;
						default: goto end;
					}

					temp += 2;
				}

				end:
				temp--;
			}
			else if(strcmp((uint8 *)string, (uint8 *)"init")==0)
			{
				if(*((uint32 *)OS_INFO) == INTERRUPTS_ENABLED)
				{
					Print((uint8 *)"\n\tInterrupts already enabled\n\n", WHITE, BLACK);
					goto finish;
				}
				//idt_init();

				uint32 r = 0;

				/*__asm ("movl $17, %%eax;"
				       "movl $0, %%ebx;"
				       //"movl $0, %%edx;"
				       "divl %%ebx;"
				       : : :
				      );*/
			//	PrintHex(r);

				// Set some "flags"
				L_OS_INFO_ADDR -= 2;
				*L_OS_INFO_ADDR = INTERRUPTS_ENABLED;
				L_OS_INFO_ADDR  += 2;
				*L_OS_INFO_ADDR = EMPTY;
				//L_OS_INFO_ADDR += 2;
			}
			else if(strcmp((uint8 *)string, (uint8 *)"reboot")==0)
			{
				tc.cursor_y = 0;
				tc.cursor_x = 0;
				__asm ("int $0x03");
			}
			else if(strcmp((uint8 *)string, (uint8 *)"clear")==0)
				clear_screen();
			else Print((uint8 *)"\n\t\t Unknown Command ", BLACK, RED);

finish:
			for(uint32 i = 0; i < length; i++)
				string[i] = 0;
			length = 0;
			
			Print((uint8 *)"\n", BLACK, BLACK);
			PutC((uint8)'>', WHITE, BLACK);
			continue;
		}
	}
}

#endif

#ifndef TERM
#define TERM

void terminal()
{
	Print((uint8 *)"Welcome to MocaOS!\n", WHITE, BLACK);
	Print((uint8 *)"\tTo Get Started:\n", WHITE, BLACK);
	Print((uint8 *)"\t\t * init heap TYPE(Where TYPE is Normal, Medium, Large or ELarge) \n\t\t * init sudo user(allows super access to terminal functionality) \n\t\t * init settings(go through all terminal settings, eg. Terminal color, Terminal cursor etc) \n", BLACK, MakeColor(119, 255, 91));
	Print((uint8 *)"\t\t * OR type init to see all available choices \n\n", WHITE, BLACK);
	PutC((uint8)'>', WHITE, BLACK);

	uint8 input;
	uint8 string[10];
	uint32 length = 0;

	while(1)
	{
		input = get_key();

		if(input && !(input == 0x1C))
		{
			PutC((uint8)input, WHITE, BLACK);
			string[length] = input;
			length++;

			continue;
		}
		if(input == 0x1C)
		{
			Print((uint8 *)string, WHITE, BLACK);
			
			for(uint32 i = 0; i < length; i++)
				string[i] = '\0';
			length = 0;
			
			Print((uint8 *)"\n", BLACK, BLACK);
			PutC((uint8)'>', WHITE, BLACK);
			continue;
		}
	}
}

#endif

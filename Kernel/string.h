#ifndef MocaOS_STRING
#define MocaOS_STRING

size_t strlen(uint8 *string)
{
	size_t length = 0;

	while(string[length] != 0)
		length++;
	
	return length;
}

uint8 strcmp(uint8 *str1, uint8 *str2)
{	
	if(strlen(str1) != strlen(str2))
		return 1;

	for(size_t i = 0; i < strlen(str1); i++)
	{
		if(str1[i] != str2[i]) return 1;
		if(i == strlen(str1)) return 0;
	}

	return 0;
}

#endif

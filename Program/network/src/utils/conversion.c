

#include "conversion.h"





void inet_ntoa(char *str_addr, const unsigned int addr)
{
	sprintf(str_addr, "%d.%d.%d.%d", (addr>>24)&0xff,
								(addr>>16)&0xff,
								(addr>>8)&0xff,
								(addr>>0)&0xff);
}

unsigned int inet_aton(const char *str_addr)
{
	unsigned int part1,part2,part3,part4;
	unsigned int intip = 0;
	if(sscanf(str_addr, "%u.%u.%u.%u", &part1, &part2, &part3, &part4) == 4)
	{
		intip = (part1 << 24) + (part2 << 16) + (part3 << 8) + part4;
	}

	return 	intip;
}




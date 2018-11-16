

#include "conversion.h"





void inet_ntoa(char *str_addr, const unsigned int addr)
{
	sprintf(str_addr, "%d.%d.%d.%d", (addr>>24)&0xff,
								(addr>>16)&0xff,
								(addr>>8)&0xff,
								(addr>>0)&0xff);
}





#include "conversion.h"


char *inet_ntoa(const unsigned int addr)
{
	char *ip=NULL;

	ip= (char *)malloc(16);
	sprintf(ip, "%d.%d.%d.%d", (addr>>24)&0xff,
								(addr>>16)&0xff,
								(addr>>8)&0xff,
								(addr>>0)&0xff);
	return ip;
}



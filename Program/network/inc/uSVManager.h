#ifndef __USVMANAGER_H
#define __USVMANAGER_H

#include "uCommon.h"


#include "uLLPacket.h"


struct USVMANAGER_CONTROL{
	unsigned int flag;

	unsigned int scan_count;
	unsigned int scan_count_cur;

	struct FRAME_DAT readly_to_send;
};


RES_Typedef uSVManagerInit(void);
RES_Typedef uSVManagerDiscoveryStart(void);
RES_Typedef uSVManagerDiscoveryStop(void);

#endif


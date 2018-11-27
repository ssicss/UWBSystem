
#ifndef _ULLPACKET_H
#define _ULLPACKET_H

#include "uCommon.h"

#include "uLLDriver.h"



RES_Typedef uLLFrameSendEx(const MTYPE_Typedef mtype,
										const SUBTYPE_Typedef subtype,
										const unsigned int addr,
										const bool hd,
										const unsigned int len,
									 	char *data);

SUBTYPE_Typedef uLLFrameWait(const unsigned int retry);

RES_Typedef uLLFrameWaitEx(const MTYPE_Typedef mtype,
							const SUBTYPE_Typedef subtype,
							const unsigned int addr,
							const unsigned int retry);

SUBTYPE_Typedef uLLFrameWaitInAddr(const unsigned int addr,
										const unsigned int retry);

struct FRAME_DAT *uLLFrameWaitData(const MTYPE_Typedef mtype,
							const SUBTYPE_Typedef subtype,
							const unsigned int addr,
							const unsigned int retry);

SUBTYPE_Typedef uLLFrameWaitInAddrEx(const unsigned int addr,
											const unsigned int retry,
											char *data);



RES_Typedef uLLFrameWaitExNoPre(const MTYPE_Typedef mtype,
					const SUBTYPE_Typedef subtype,
					const unsigned int addr,
					const unsigned int retry);

#endif


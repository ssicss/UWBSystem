
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

#endif


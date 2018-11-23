
#ifndef _USTATEMAINTENANCE_H
#define _USTATEMAINTENANCE_H

#include "uCommon.h"
#include "uLLDriver.h"
#include "uLLPacket.h"

RES_Typedef uPTPing(struct FRAME_DAT *frame);
RES_Typedef uPTPingRespons(void);
RES_Typedef uPTGetConfigure(void);
RES_Typedef uPTGetConfigureRespons(void);
RES_Typedef uPTSetConfigure(struct FRAME_DAT *frame);
RES_Typedef uPTSetConfigureRespons(char *buf);




#endif


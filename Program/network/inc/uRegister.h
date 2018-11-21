#ifndef _UREGISTER_H
#define _UREGISTER_H

#include "uCommon.h"

#include "uLLDriver.h"
#include "uLLPacket.h"


#define SAVE_RANDOM_ADDR_MAX (100)




//等待设备注册超时重试次数
#define WAITDEV_REGISTER_RETRY (100)    //


extern struct list *guSVManagerDevice;


RES_Typedef uPTRegisterCycle(void);
RES_Typedef uPTRegisterRespons(void);
RES_Typedef uPTSigninRespons(void);


#endif



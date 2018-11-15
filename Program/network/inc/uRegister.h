#ifndef _UREGISTER_H
#define _UREGISTER_H

#include "uCommon.h"

#include "uLinklayer.h"
#include "conversion.h"
#include "uSVManager.h"


#define SAVE_RANDOM_ADDR_MAX (1024)

//分配地址池
#define ADDR_POOL_LEN (100)
#define ADDR_POOL_PREFIX (0xc0a80164)

//等待设备注册超时时间
#define WAITDEV_REGISTER_TIMEOUT (9262)  //大概50个周期

//等待设备注册超时重试次数
#define WAITDEV_REGISTER_RETRY (100)    //


#define _PCK(x) if(x){return RES_PACKET_ERROR;}

 struct DevAddrPool{
	unsigned int addr;
	bool _is_vaild;
};

extern struct DevAddrPool gAddrPool[ADDR_POOL_LEN];

RES_Typedef uPTRegisterCycle(void);
RES_Typedef uPTRegisterRespons(void);


#endif



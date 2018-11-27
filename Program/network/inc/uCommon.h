
#ifndef __UCOMMON_H
#define __UCOMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "kdwm1000_bsp.h"
#include "modules\dw1000.h"

#include "stdplus.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"


#include "conversion.h"	

#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436


#define POLL_RX_TO_RESP_TX_DLY_UUS  1100
#define UUS_TO_DWT_TIME 65536



#define _PCK(x) if(x){return RES_PACKET_ERROR;}



#define USE_ASSERT

//分配地址池
#define ADDR_POOL_LEN (100)
#define ADDR_POOL_START (0xc0a80164)

 //等待设备注册超时时间
#define WAITDEV_REGISTER_TIMEOUT (9262)  //大概50个周期


 struct DevAddrPool{
	unsigned int addr;
	bool _is_vaild;
};


typedef enum{
		RES_OK = 0,


		RES_LL_PRAMAS_INVALD = 1,
		RES_LL_MEMORY_FAILED = 2,
		RES_LL_USER_DEFINED_FAILED = 3,
		RES_LL_PRASE_FAILED = 4,
		RES_LL_TIMEOUT = 5,

		RES_PT_PRAMAS_INVALD = 4,
		RES_PT_MEMORY_FAILED = 5,
		RES_PT_PRASE_FAILED = 6,
		RES_PT_TIMEOUT = 7,

		



		
		RES_PRAMAS_INVALD = 1,
		RES_RETRY_FAILED = 2,
		RES_RECV_FLAG_FAILED = 3,
		RES_RECV_DATA_TOOLARGE = 4,
		RES_MALLOC_FAILED = 5,
		RES_RARSE_FAILED = 6,

		RES_PACKET_ERROR = 7,
		RES_OPT_ERROR = 8,

		RES_SVNANAGER_SERVER_CANCEL = 9,

		RES_CREATE_QUEUE_ERROR = 10,
		RES_TIMEOUT = 11,
		RES_SYS_NOINIT = 12,
}RES_Typedef;

typedef enum{
	DEV_ROLE_BS = 0,
	DEV_ROLE_LABEL = 1,
}DEV_ROLE_Typedef;
	
typedef enum{
	DEV_NROLE_AP = 0,
	DEV_NROLE_STA = 1,
}DEV_NROLE_Typedef;


	
struct MANAGER_DEV_INFO{
	unsigned int ip;
	unsigned int sn;
	DEV_ROLE_Typedef role;
	DEV_NROLE_Typedef nrole;
	unsigned int priority;
};


#ifdef USE_ASSERT
	#define uassert(expr) if(!expr){ SEGGER_RTT_printf(0, " assert failed<%s>:%d\n\r", __FILE__, __LINE__); for(;;); }
#else
	#define uassert(expr)   ((void)0)
#endif

#endif

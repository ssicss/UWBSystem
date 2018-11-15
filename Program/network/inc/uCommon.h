
#ifndef __UCOMMON_H
#define __UCOMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "kdwm1000_bsp.h"
#include "modules\dw1000.h"


typedef enum{
		RES_OK = 0,
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



#endif

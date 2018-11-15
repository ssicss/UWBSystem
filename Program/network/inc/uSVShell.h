#ifndef __USVSHELL_H
#define __USVSHELL_H

#include "uCommon.h"

extern struct QUEUE *gUartQueue;
extern bool _is_uart_handle;

RES_Typedef uSVShellInit(void);
RES_Typedef uSVShellPrase(void);


#endif


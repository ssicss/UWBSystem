
#include "drivers\stm32f4_system.h"


#include "ranging.h"
#include "uLinklayer.h"
#include <stdio.h>
#include "uRegister.h"
#include "uIdle.h"
#include "uSVShell.h"
#include "uSVManager.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"


int main( void )
{
	RES_Typedef res;

	HAL_InitTick();

	RangingInit();

	uSVShellInit();

	if(uLLInit() != RES_OK)
		SEGGER_RTT_printf(0,"ll init error\r\n");

	uSVManagerInit();
	
	SEGGER_RTT_Init();
	SEGGER_RTT_printf(0, "aaaaaaaaaaaaaaaaaaa\n\r");

	
	for(;;)
	{
		res = uSVIdle();
		if(res){
			printf("exit code:%d\n\r", res);
			for(;;);
		}
	}
}


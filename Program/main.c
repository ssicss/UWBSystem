
#include "drivers\stm32f4_system.h"


#include "ranging.h"
#include "uLinklayer.h"
#include <stdio.h>
#include "uRegister.h"
#include "uIdle.h"
#include "uSVShell.h"
#include "uSVManager.h"

int main( void )
{
	RES_Typedef res;

	HAL_InitTick();

	RangingInit();

	uSVShellInit();

	if(uLLInit() != RES_OK)
		printf("ll init error\r\n");

	uSVManagerInit();
	

	
	for(;;)
	{

		res = uSVIdle();
		if(res){
			printf("exit code:%d\n\r", res);
			for(;;);
		}
	}
}


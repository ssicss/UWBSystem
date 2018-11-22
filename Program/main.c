
#include "drivers\stm32f4_system.h"


#include "ranging.h"
#include "uLLdriver.h"
#include <stdio.h>
#include "uRegister.h"
#include "uIdle.h"
#include "uSVShell.h"
#include "uSVManager.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"






//uLLUserInterfaceRegister()


bool DeviceInit(void)
{


	return true;
}
void DeviceDelayMs(size_t t)
{
	delay_ms(t);
}


bool DeviceSend(const char *buf, size_t len)
{

	unsigned int status = 0;
	unsigned int t = 0;

	//清标志位
	DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_TXFRS|
										  SYS_STATUS_TXPHS|
											SYS_STATUS_TXPRS|
											SYS_STATUS_TXFRB);

	//发送
	dwt_writetxdata(len+2, (unsigned char *)buf, 0);
	dwt_writetxfctrl(len+2, 0);	
	
	dwt_starttx(DWT_START_TX_IMMEDIATE);   //立即发送，延时发送？发送后打开接收器？ 暂未完善

	//等待发送完成
	do {
		status = DW1000_readData32(SYS_STATUS_ID, 0);
		t++;
	} while((!(status & (SYS_STATUS_TXFRS)))&&(t<100));
	if(t>=100)
	{
		SEGGER_RTT_printf(0, "send timeout\r\n");
		return false;
	}	

	return true;
}

size_t DeviceRecv(char *buf, size_t timeout)
{
	unsigned int status = 0;
	unsigned int data_len = 0;

	dwt_setrxtimeout(timeout);
	dwt_rxenable(0);
	do {
		status = DW1000_readData32(SYS_STATUS_ID, 0);
	} while(!(status & (SYS_STATUS_RXFCG  |SYS_STATUS_ALL_RX_ERR)));
	//SYS_STATUS_RXFCG   帧CRC校验成功
	//SYS_STATUS_ALL_RX_ERR  接收时所有可能的出错情况

	if(!(status & SYS_STATUS_RXFCG))
	{
	    DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_ALL_RX_ERR);
	    dwt_rxreset();
		return 0;
	}
		
	DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_RXFCG);
 
	data_len = DW1000_readData32(RX_FINFO_ID, 0) & RX_FINFO_RXFL_MASK_1023;
	if(data_len > DEV_DATA_LEN_MAX){
		return 0;
	}
	dwt_readrxdata((unsigned char *)buf, data_len, 0);

	return data_len;
}




int main( void )
{
	RES_Typedef res;
	struct uLLUserInterface uUserInter;


	HAL_InitTick();

	RangingInit();

	uUserInter.uLLDeviceInit = DeviceInit;
	uUserInter.uLLDeviceDelayMs = DeviceDelayMs;
	uUserInter.uLLDeviceSend = DeviceSend;
	uUserInter.uLLDeviceRecv = DeviceRecv;
	
	if(uLLUserInterfaceRegister(&uUserInter) != RES_OK){
		printf("register err\n\r");
		for(;;);
	}

	uSVShellInit();

	if(uLLInit() != RES_OK)
		SEGGER_RTT_printf(0,"ll init error\r\n");

	uSVManagerInit();
	
	SEGGER_RTT_Init();
	SEGGER_RTT_printf(0, "bbbb\n\r");

	
	for(;;)
	{
		res = uSVIdle();
		if(res){
			printf("exit code:%d\n\r", res);
			for(;;);
		}
	}
}


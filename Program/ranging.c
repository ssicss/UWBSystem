

#include "ranging.h"
#include "kdwm1000_bsp.h"
#include "modules\serial.h"
#include "modules\dw1000.h"
#include "stdplus.h"

/* Default antenna delay values for 64 MHz PRF. See NOTE 2 below. */
#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436


static dwt_config_t config = {
  2,               /* Channel number. */
  DWT_PRF_64M,     /* Pulse repetition frequency. */
  DWT_PLEN_128,    /* Preamble length. */
  DWT_PAC8,        /* Preamble acquisition chunk size. Used in RX only. */
  9,               /* TX preamble code. Used in TX only. */
  9,               /* RX preamble code. Used in RX only. */
  0,               /* Use non-standard SFD (Boolean) */
  DWT_BR_6M8,      /* Data rate. */
  DWT_PHRMODE_STD, /* PHY header mode. */
  (129 + 8 - 8)    /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

void RangingInit(void)
{
	KDWM_GPIO_Config();
	KDWM_UART_Config(NULL);
	KDWM_DW1000_Config();

	DW1000_Reset();
	dwt_initialise(DWT_LOADUCODE);
	dwt_configure(&config);
	
	/* Apply default antenna delay value. See NOTE 2 below. */
	DW1000_SetAntennaDelayRX(RX_ANT_DLY);
	DW1000_SetAntennaDelayTX(TX_ANT_DLY);

}

static uint32_t status = 0;
static uint8_t rx_buffer[1024]   = {0};

void RangingLoop(void)
{
	unsigned int i=0;
	struct QUEUE *qData = NULL;
	size_t item = 0;
	
	qData = QueueCreate(1024);
	if(qData == NULL){
		printf("create queue err\r\n");
		while(1);
	}


	
	while(1)
	{
		dwt_rxenable(0);
		
		do {
			
			while(!QueueIsEmpty(qData)){
				if(QueuePop(qData, &item)){
					printf(" %02x\r\n", item);
				}else{
					printf("pop err\r\n");
				}
			}
		
		  status = DW1000_readData32(SYS_STATUS_ID, 0);
		} while(!(status & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)));
		printf("\r\n");

		if(status & SYS_STATUS_RXFCG) {
		  uint32_t frame_len;
		
		  DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_RXFCG);
		  frame_len = DW1000_readData32(RX_FINFO_ID, 0) & RX_FINFO_RXFL_MASK_1023;
		  if(frame_len <= 1024) { 
			dwt_readrxdata(rx_buffer, frame_len, 0);


			for(i=0; i<frame_len; i++)
			{
				
				if(!QueuePush(qData, rx_buffer[i]&0xff))
					printf("push err\r\n");

			}
		  
			memset(rx_buffer, 0, frame_len);
		  }else{
			printf("frame len error\r\n");
		  }

		}else{
			 // Serial_SendStr("status err\n");
		}
		
		
	}



}



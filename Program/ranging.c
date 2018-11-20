

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



#include "uPTRanging.h"



RES_Typedef _uPTRangingSendPoll(void)
{
	struct FRAME_DAT	poll;

	poll.mtype = MTYPE_RANGING;
	poll.subtype = SUBTYPE_POLL;
	poll.addr = 0;
	poll.hd = false;
	poll.len = 0;
	poll.data = NULL;
	
	uLLFrameSendExpResp(&poll);

	return RES_OK;
}
RES_Typedef _uPTRangingSendResp(void)
{
	struct FRAME_DAT	resp;

	resp.mtype = MTYPE_RANGING;
	resp.subtype = SUBTYPE_RESPONS;
	resp.addr = 0;
	resp.hd = false;
	resp.len = 0;
	resp.data = NULL;

	uLLFrameSendDelayed(&resp, 0);

	return RES_OK;
}


RES_Typedef _uPTRangingWaitBSResp(void)
{
	return uLLFrameWaitExNoPre(MTYPE_RANGING, SUBTYPE_RESPONS, 0, 100);
}





RES_Typedef _uPTRangingWaitRespons(void)
{
	if(uLLFrameWaitEx(MTYPE_RANGING,SUBTYPE_POLL,0, 100) == RES_OK)
	{

		
		//SEGGER_RTT_printf(0, "time :%llu\n\r", uLLGetRxTimeStamp() - uLLGetTxTimeStamp());

		
		return RES_OK;
	}else{
		return RES_PT_TIMEOUT;
	}	
}

RES_Typedef uPTRangingRespons(void)
{

	if(_uPTRangingSendPoll() == RES_OK)
	{
		return RES_OK;

		/*
		if(_uPTRangingWaitBSResp == RES_OK)
		{
			printf("time :%llu\n\r", uLLGetRxTimeStamp() - uLLGetTxTimeStamp());
			return RES_OK;
		}

		*/
	}
	return RES_PT_TIMEOUT;
}

RES_Typedef uPTRangingRespinsHandle(char *buf)
{
	unsigned long long t;

	t = uLLGetRxTimeStamp() - uLLGetTxTimeStamp();
	
	uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, true, sizeof(t), (char *)&t );
	return RES_OK;
}


#define SPEED_OF_LIGHT 299702547


	unsigned long long t1 = 0;
	unsigned  long long t2 = 0;
	unsigned  long long t3 = 0;



RES_Typedef uPTRangingCyle(struct FRAME_DAT *frame)
{
	struct FRAME_DAT *dat;
	double tof;
	double distanc;

	unsigned int resp_tx_time;


	for(;;)
	{



		if(uLLFrameSend(frame) == RES_OK){
		
			
			if(_uPTRangingWaitRespons()== RES_OK){
				t1 = uLLGetRxTimeStamp();
				//printf("catch poll \n\r");
			




				_uPTRangingSendResp();

				resp_tx_time = (t1 + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
				t2 = (((unsigned long long)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;




				
				//t2 = uLLGetTxTimeStamp();

				dat = uLLFrameWaitData(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, 100);
				if(dat){
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
					t3 = (*((unsigned long long *)dat->data))& 0xffffffff;
					//SEGGER_RTT_printf(0, "t1:%15llu , t2:%15llu, t3:%15llu\n\r",t1, t2, t3);

					

				      tof = (((double)t3 - ((double)t2-(double)t1)) / 2.0f)*DWT_TIME_UNITS ; // Specifying 1.0f and 2.0f are floats to clear warning 
     				 distanc = tof * SPEED_OF_LIGHT;


					 printf( "dist:%f,  t3:%15llu,  t2-t1:%15llu,\n\r", distanc,  t3,  t2- t1);

				}else{
					printf("wait data timeout\n\r");
				}
				
			}else{
				printf("wait timeout\n\r");
			}
		}

		uLLDelayMs(200);

	}


	return RES_OK;

}



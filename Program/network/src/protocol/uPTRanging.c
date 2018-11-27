
#include "uPTRanging.h"

extern struct list *guSVManagerDevice;

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
RES_Typedef _uPTRangingSendResp(unsigned int addr)
{
	struct FRAME_DAT	resp;

	resp.mtype = MTYPE_RANGING;
	resp.subtype = SUBTYPE_RESPONS;
	resp.addr = addr;
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
	if(uLLFrameWaitEx(MTYPE_RANGING,SUBTYPE_POLL,0, 20) == RES_OK)
	{

		
		//SEGGER_RTT_printf(0, "time :%llu\n\r", uLLGetRxTimeStamp() - uLLGetTxTimeStamp());

		
		return RES_OK;
	}else{
		return RES_PT_TIMEOUT;
	}	
}


unsigned long long t_poll;
unsigned long long t_resp;

RES_Typedef uPTRangingRespons(unsigned int addr)
{
	for(;;){



		_uPTRangingSendPoll();
		
		if(uLLFrameWaitEx(MTYPE_RANGING, SUBTYPE_RESPONS, addr, 100) != RES_OK){
			goto end;
		}

		uPTRangingRespinsHandle(NULL);
		

		end:

		if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_RANGING_REQUEST, addr, 400) != RES_OK){
			goto end;
		}
	}

}

RES_Typedef uPTRangingRespinsHandle(char *buf)
{
	unsigned long long t;
	t_poll = uLLGetTxTimeStamp();

	t_resp = uLLGetRxTimeStamp(); 

	t = t_resp - t_poll;
	//printf("t:%llu, tpoll:%llu,tresp:%llu\n\r", t,t_poll, t_resp );
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
	char buf[16];


	struct listnode *node = NULL;
	unsigned int i=0;
	struct MANAGER_DEV_INFO *dev;
for(;;)
{

	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{

		if(i>0){
			dev = (struct MANAGER_DEV_INFO *)node->data;

		if(uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_REQUEST, dev->ip, false, 0, NULL) == RES_OK){
			if(_uPTRangingWaitRespons()== RES_OK){
				t1 = uLLGetRxTimeStamp();
			
				inet_ntoa(buf, dev->ip);
				SEGGER_RTT_printf(0, "<%s>catch poll \n\r", buf);
	
				_uPTRangingSendResp(dev->ip);
				resp_tx_time = (t1 + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
				t2 = (((unsigned long long)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;


				//t2 = uLLGetTxTimeStamp();

				dat = uLLFrameWaitData(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, 20);
				if(dat){
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
					t3 = (*((unsigned long long *)dat->data))& 0xffffffff;
					SEGGER_RTT_printf(0, "t3:%15llu\n\r",t3);
					//printf( "t2-t1:%llu,t3:%llu\n\r",t2-t1, t3 );

					

				      tof = (((double)t3 - ((double)t2-(double)t1)) / 2.0f)*DWT_TIME_UNITS ; // Specifying 1.0f and 2.0f are floats to clear warning 
     				 distanc = tof * SPEED_OF_LIGHT;

					 
					 if(distanc<100){
						 printf( "<%d>%10.3f	", i, distanc);

					 }
					

				}else{
					//printf("wait data timeout\n\r");
				}
				
			}else{
				//printf("wait timeout\n\r");
			}
		}



		}

		node = node->next;

		uLLDelayMs(10);
	}

	printf("\n\r");

	//uLLDelayMs(500);
	

}
	return RES_OK;

}



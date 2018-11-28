
#include "uPTRanging.h"

extern struct list *guSVManagerDevice;


unsigned long long t1 = 0;
unsigned  long long t2 = 0;
unsigned  long long t3[100];


RES_Typedef _uPTRangingSendPoll(unsigned int addr)
{
	struct FRAME_DAT	poll;

	poll.mtype = MTYPE_RANGING;
	poll.subtype = SUBTYPE_POLL;
	poll.addr = 0;
	poll.hd = true;
	poll.len = sizeof(unsigned int);
	poll.data = (char *)&addr;
	
	uLLFrameSendExpResp(&poll);

	return RES_OK;
}
RES_Typedef _uPTRangingSendResp(unsigned int addr, unsigned int delay)
{
	struct FRAME_DAT	resp;

	resp.mtype = MTYPE_RANGING;
	resp.subtype = SUBTYPE_RESPONS;
	resp.addr = addr;
	resp.hd = true;
	resp.len = sizeof(unsigned long long);
	resp.data = (char *)&t1;

	uLLFrameSendDelayed(&resp, delay);

	return RES_OK;
}


RES_Typedef _uPTRangingWaitBSResp(void)
{
	return uLLFrameWaitExNoPre(MTYPE_RANGING, SUBTYPE_RESPONS, 0, 100);
}





RES_Typedef _uPTRangingWaitRespons(unsigned int *addr)
{
	struct FRAME_DAT *redata;

	redata = uLLFrameWaitData(MTYPE_RANGING,SUBTYPE_POLL,0, 20);

	if(redata)
	{
		*addr = *(unsigned int *)redata->data;
		//SEGGER_RTT_printf(0, "time :%llu\n\r", uLLGetRxTimeStamp() - uLLGetTxTimeStamp());

		
		return RES_OK;
	}else{
		return RES_PT_TIMEOUT;
	}	
}

struct RANGING_DAT_RETURN{
	unsigned long long dat;
	unsigned int index;
};

struct RANGING_DAT_RETURN gRangingReturn;


unsigned long long t_poll;
unsigned long long t_resp[64];
unsigned long long t_getpoll[64];


RES_Typedef uPTRangingRespons(unsigned int addr, char *cbuf)
{
	struct listnode *node = NULL;
	struct MANAGER_DEV_INFO *dev;
//	unsigned int resp_tx_time;
	char buf[16];
	unsigned int i=0;
	unsigned int count =0;
	unsigned long long t;
	unsigned int readdr;
	struct FRAME_DAT *fram;
	unsigned int resp_tx_time;

	uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_BEGIN_RESPONS, 0, false, 0, NULL);
	
	count  = *(unsigned int *)cbuf;

	
	node = guSVManagerDevice->head;
	dev = (struct MANAGER_DEV_INFO *)node->data;

	if(dev->role == DEV_ROLE_BS){
		for(;;){
			if(_uPTRangingWaitRespons(&readdr)== RES_OK){
				t1 = uLLGetRxTimeStamp();
			
			//	inet_ntoa(buf, dev->ip);
			//	SEGGER_RTT_printf(0, "<%s>catch poll \n\r", buf);

				_uPTRangingSendResp(readdr, dev->priority);
				//resp_tx_time = (t1 + (POLL_RX_TO_RESP_TX_DLY_UUS * (1+dev->priority) * UUS_TO_DWT_TIME)) >> 8;
				//t2 = (((unsigned long long)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
				
				
			}else{
				//printf("wait timeout\n\r");
			}
			
		}

	}else{

		for(;;){
end:
			
			if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_RANGING_REQUEST, addr, 400) != RES_OK){
				goto end;
			}

			memset(t_resp, 0, sizeof(t_resp));
			_uPTRangingSendPoll(dev->ip);


			for(i=0; i<count; i++){
				fram = uLLFrameWaitData(MTYPE_RANGING, SUBTYPE_RESPONS, addr, 200);
				if( fram == NULL){
					t_resp[i] = 0;
					t_getpoll[i] = 0;
				}else{
					t_resp[i] = uLLGetRxTimeStamp(); 
					t_getpoll[i] = *(unsigned long long *)fram->data;
				}
				
			}
			t_poll = uLLGetTxTimeStamp();


			for(i=0; i<count; i++){
				t = t_resp[i] - t_poll;
	
				resp_tx_time = (t_getpoll[i] + (POLL_RX_TO_RESP_TX_DLY_UUS * (1+i) * UUS_TO_DWT_TIME)) >> 8;
				t2 = (((unsigned long long)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;

				
				gRangingReturn.index = i;
				gRangingReturn.dat = t - (t2 - t_getpoll[i]);
				//printf("t:%llu, tpoll:%llu,tresp:%llu\n\r", t,t_poll, t_resp );
				uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, true, sizeof(struct RANGING_DAT_RETURN), (char *)&gRangingReturn );
				uLLDelayMs(10);
			}

			//uPTRangingRespinsHandle(NULL);


		}
	}
	

}

RES_Typedef uPTRangingRespinsHandle(char *buf)
{
	
	/*
	unsigned long long t;
	t_poll = uLLGetTxTimeStamp();

	t = t_resp - t_poll;
	//printf("t:%llu, tpoll:%llu,tresp:%llu\n\r", t,t_poll, t_resp );
	uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, true, sizeof(t), (char *)&t );
	return RES_OK;
	
	
	*/
}


#define SPEED_OF_LIGHT 299702547



unsigned int GetDevSn(unsigned int index)
{
	struct listnode *node = NULL;
	struct MANAGER_DEV_INFO *dev;
	unsigned int i=0;

	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		if(i==index)
			break;
		node = node->next;
	}
	
	dev = (struct MANAGER_DEV_INFO *)node->data;

	return dev->sn;
}


RES_Typedef uPTRangingCyle(struct FRAME_DAT *frame)
{
	struct FRAME_DAT *dat;
	double tof;
	double distanc;

	unsigned int resp_tx_time;
	char buf[16];


	struct listnode *node = NULL;
	unsigned int i=0;
	unsigned int j=0;
	struct MANAGER_DEV_INFO *dev;

	unsigned int count=0;

	unsigned int tmp;
	double  tmp2;
	unsigned int q= 0;
	unsigned int readdr;

	char buf_send[23];

	count = 0;
	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		dev = (struct MANAGER_DEV_INFO *)node->data;
		if(dev->role == DEV_ROLE_BS)
			count++;

		node = node->next;
	}



	SEGGER_RTT_printf(0, "bs:%d\n\r", count);

	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{

		if(i>0){
			dev = (struct MANAGER_DEV_INFO *)node->data;
			uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_BEGIN_REQUEST, dev->ip, true, sizeof(count), (char *)&count);

			if(uLLFrameWaitEx(MTYPE_NT,SUBTYPE_RANGING_BEGIN_RESPONS,0, 20) == RES_OK)
			{
				SEGGER_RTT_printf(0, "<%d>ready\n\r", i);
			}else{
				SEGGER_RTT_printf(0, "<%d>no ask\n\r", i);
			}
			

			
		}
		node = node->next;
	}



	uLLDelayMs(500);
	SEGGER_RTT_printf( 0, "ready to ranging\n\r");

	for(;;)
	{

	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		SEGGER_RTT_printf(0, "scan %d\n\r ", i);

		if(i>0){
			dev = (struct MANAGER_DEV_INFO *)node->data;
			

			if(dev->role == DEV_ROLE_LABEL){

				//SEGGER_RTT_printf( 0, "send randing request\n\r");
				if(uLLFrameSendEx(MTYPE_NT, SUBTYPE_RANGING_REQUEST, dev->ip, false, 0, NULL) == RES_OK){

					//SEGGER_RTT_printf( 0, "wait poll\n\r");
					if(_uPTRangingWaitRespons(&readdr)== RES_OK){
						t1 = uLLGetRxTimeStamp();
					
						inet_ntoa(buf, dev->ip);
						//SEGGER_RTT_printf(0, "<%s>catch tag poll, send resp - %d\n\r", buf, dev->priority);
						_uPTRangingSendResp(dev->ip, dev->priority);

						for(j=0; j<count; j++){
							//SEGGER_RTT_printf( 0, "wait data return\n\r");
							dat = uLLFrameWaitData(MTYPE_NT, SUBTYPE_RANGING_DATRETURN, 0, 20);
							if(dat){			
								t3[j] = (*((unsigned long long *)dat->data))& 0xffffffff;
								//SEGGER_RTT_printf(0, "t3:%15llu\n\r",t3);
							}else{
								t3[j] = 0;
								//SEGGER_RTT_printf(0, "wait data return timeout\n\r");
							}

						}	

						
						for(j=0; j<count; j++){
								if(t3[j]){
									//resp_tx_time = (t1 + (POLL_RX_TO_RESP_TX_DLY_UUS * (1+j) * UUS_TO_DWT_TIME)) >> 8;
									//t2 = (((unsigned long long)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;
											
			
									//printf("tag<%d>- t1:%llu, t2:%llu, t3:%llu   ", j, t1,t2,t3[j]);
									
									
									tof = ((double)t3[j]/2.0f )*DWT_TIME_UNITS ; // Specifying 1.0f and 2.0f are floats to clear warning 
									distanc = tof * SPEED_OF_LIGHT;

									buf_send[0] = 0xfa;
									buf_send[1]=0x01;
									buf_send[2] = 0x14;
									buf_send[3] = 0;
									buf_send[4] = 0;
									buf_send[5] = 0;
									buf_send[6] = 0;

									SEGGER_RTT_printf(0, "j=%d ", j);

									memcpy(&buf_send[7], &j, 4);


									tmp = GetDevSn(i);
									SEGGER_RTT_printf(0, "tmp=%d ", tmp);
									memcpy(&buf_send[11], &tmp, 4);
									tmp2 = distanc > 100 ? 0 : distanc;
									memcpy(&buf_send[15], &tmp2 , sizeof(double));
									
									Serial_SendData(buf_send, sizeof(buf_send));


									//printf( "\xfa\x01\x14\x0\x0\x0\x0\x      <group:0><bs:%d><tag:%d><dt:%lf>", j,	GetDevSn(i) ,distanc > 100 ? 0 : distanc );


									//printf( "<group:0><bs:%d><tag:%d><dt:%lf>", j,	GetDevSn(i) ,distanc > 100 ? 0 : distanc );

										//printf( "<%d>0", j );
										//printf( "\n\r-----<%d>lost-%10.3f, (t2-t1):%lf, t3:%lf-----\n\r",j, distanc, ((double)t2-(double)t1), t3[j]);
	

								}else{
									//printf( "<!!!%d>%10.3f, t3=%llu");	
								}

						}

						//SEGGER_RTT_printf(0, "\n\r");


						
					}else{
						//printf("wait timeout\n\r");
					}
				}
			}else{
				SEGGER_RTT_printf(0, "scan bs\n\r ");
			}
		}

		node = node->next;
	}

	//printf("\n\r");

	//uLLDelayMs(500);
	

}
	return RES_OK;

}




#include "uStateMaintenance.h"



static RES_Typedef _uPTWaitRespons(SUBTYPE_Typedef request)
{
	RES_Typedef res;
	struct FRAME_DAT fra_wt;
	unsigned int t=0;


	do{
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
		t++;
	}while((t<100)&&(res!=RES_OK));
		
	if(t>=100){
		return RES_TIMEOUT;
	}
	

	if((fra_wt.mtype == MTYPE_NT)&&
			(fra_wt.subtype == request))
	{
		return RES_OK;
	}else{
		return RES_RARSE_FAILED;
	}

}

RES_Typedef uPTPing(struct FRAME_DAT *frame)
{
	unsigned int i=0;
	char ip_addr[16];
	unsigned int j=0;

	if(!frame)
		return RES_PRAMAS_INVALD;
	
	inet_ntoa(ip_addr, frame->addr);
	printf("Ping %s ..\n\r", ip_addr);
	for(i=0; i<5; i++){
		
		
		_PCK(uLLFrameSend(frame));
       if(_uPTWaitRespons(SUBTYPE_PING_RESPONS) == RES_OK)
       {
			printf("<%d>catch respons from host\n\r", i);
			j++;
	   }else{
			printf("<%d>lost\n\r", i);
	   }
	}
	printf("Send 5 Packets, Recv %d , lost %d\n\r", j, 5-j);

	printf("#");
	
	return RES_OK;
}


RES_Typedef uPTPingRespons(void)
{
	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_PING_RESPONS, 0, false, 0, NULL));
	return RES_OK;
}





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

	if(!frame)
		return RES_PRAMAS_INVALD;
	
	inet_ntoa(ip_addr, frame->addr);
	printf("Ping %s ..\n\r", ip_addr);
	for(i=0; i<5; i++){
		
		
		_PCK(uLLFrameSend(frame));
       if(_uPTWaitRespons(SUBTYPE_PING_RESPONS) == RES_OK)
       {
			printf("<%d>catch respons from host\n\r", i);
	   
	   }else{
			printf("<%d>lost\n\r", i);
	   }

	}

	printf("#");
	
	return RES_OK;
}


RES_Typedef uPTPingRespons(void)
{

	struct FRAME_DAT ping_respons;


	if(_uPTWaitRespons(SUBTYPE_PING_REQUEST) != RES_OK){
		return RES_TIMEOUT;
	}

	ping_respons.addr=0;
	ping_respons.hd=false;
	ping_respons.len=0;
	ping_respons.data=NULL;
	ping_respons.mtype = MTYPE_NT;
	ping_respons.subtype = SUBTYPE_PING_RESPONS;
	_PCK(uLLFrameSend(&ping_respons));

	return RES_OK;
}




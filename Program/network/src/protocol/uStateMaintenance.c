              
#include "uStateMaintenance.h"


extern struct list *guSVManagerDevice;





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
       if(uLLFrameWaitEx(MTYPE_NT,SUBTYPE_PING_RESPONS,0,100) == RES_OK)
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

RES_Typedef uPTGetConfigureRespons(void)
{
	struct MANAGER_DEV_INFO *dev;
	
	dev = listnode_head(guSVManagerDevice);

	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_GET_CONFIG_RESPONS, 0, true, sizeof(struct MANAGER_DEV_INFO), (char *)dev));
	return RES_OK;
}

RES_Typedef uPTGetConfigure(void)
{

	struct FRAME_DAT request;

	
	struct listnode *node = NULL;
	unsigned int i=0;
	struct MANAGER_DEV_INFO *dev;
	struct FRAME_DAT *frame_buf;

	
	//获取
	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		if(i>0){
			dev = (struct MANAGER_DEV_INFO *)node->data;
			request.addr = dev->ip;
			request.mtype = MTYPE_NT;
			request.subtype = SUBTYPE_GET_CONFIG_REQUEST;
			request.hd = false;
			request.data = NULL;
			request.len = 0;
			
			_PCK(uLLFrameSend(&request));

			frame_buf = uLLFrameWaitData(MTYPE_NT,SUBTYPE_GET_CONFIG_RESPONS,0,100);
			if(frame_buf)
			{
				memcpy(node->data, frame_buf->data, sizeof(struct MANAGER_DEV_INFO));
				
				printf("Updata ok\n\r");
			}else{
				printf("Updata err\n\r");
			}
		}
		node = node->next;
	}


	printf("#");
	return RES_OK;

}


RES_Typedef uPTSetConfigure(struct FRAME_DAT *frame)
{
	if(!frame)
		return RES_PT_PRAMAS_INVALD;
	
	_PCK(uLLFrameSend(frame));

	if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_SET_CONFIG_RESPONS, 0, 100) == RES_OK){
		printf("set ok\n\r");
	}else{
		printf("set error\n\r");

	}
	printf("#");

		return RES_OK;
}

RES_Typedef uPTSetConfigureRespons(char *buf)
{
	struct MANAGER_DEV_INFO *dev_local;

	dev_local = listnode_head(guSVManagerDevice);
	memcpy(dev_local, buf, sizeof(struct MANAGER_DEV_INFO));

	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_SET_CONFIG_RESPONS, 0, false, 0, NULL));
	return RES_OK;
}


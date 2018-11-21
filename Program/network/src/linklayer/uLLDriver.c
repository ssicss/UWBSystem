
#include "uLLDriver.h"


static char *uLLRecvBuf = NULL;
static char *uLLSendBuf = NULL;

static struct uLLUserInterface *uUserInterface = NULL;


extern struct DevAddrPool gAddrPool[ADDR_POOL_LEN];


static RES_Typedef _uLLFrameParse(struct FRAME_DAT *obj, 
									  char *buf, 
									  const unsigned int len)
{
	unsigned char mtype = 0;
	unsigned char subtype = 0;

	if((buf[0]!=FRAME_INDENTIFICATION_BIT0)||(buf[1]!=FRAME_INDENTIFICATION_BIT1))
		return RES_LL_PRASE_FAILED;
	
	mtype = (buf[2]>>1) &0x7f;
	if(mtype == 0x0)
	{
		obj->mtype = MTYPE_NT;
	}
	else if(mtype == 0x1)
	{
		obj->mtype = MTYPE_RANGING;
	}
	else{
		return RES_LL_PRASE_FAILED;
	}

	obj->hd = (buf[2] &0x1) ? true : false;
	subtype = buf[3];
	obj->subtype = (SUBTYPE_Typedef)subtype;
	obj->addr = *((unsigned int *)&buf[4]);

	if(len > 8){
		obj->data = &buf[8];
		obj->len = len - 8;
	}
	
	return RES_OK;
}


static RES_Typedef _uLLFramePacket(const struct FRAME_DAT *obj,
									 char *buf)
{
	unsigned char mtype = 0;
	unsigned char subtype = 0;
	unsigned char hd = 0;

	buf[0] = FRAME_INDENTIFICATION_BIT0;
	buf[1] = FRAME_INDENTIFICATION_BIT1;
	if(obj->mtype == MTYPE_NT)
	{
		mtype = 0;
	}
	else if(obj->mtype == MTYPE_RANGING)
	{
		mtype = 1;
	}
	else{
		return RES_PRAMAS_INVALD;
	}

	subtype = (unsigned char)obj->subtype;
	hd  = (unsigned char)obj->hd;
	buf[2] = (mtype<<1)|hd;
	buf[3] = subtype;

	memcpy(&buf[4], &obj->addr, sizeof(unsigned int));
	
	if(obj->hd)
		memcpy(&buf[8], obj->data, obj->len);
	
	return RES_OK;
}



RES_Typedef uLLUserInterfaceRegister(struct uLLUserInterface *interface)
{
	uassert(interface);

	uUserInterface = (struct uLLUserInterface *)calloc(1,sizeof(struct uLLUserInterface));
	if(!uUserInterface)
		return RES_LL_MEMORY_FAILED;

	memcpy(uUserInterface, interface, sizeof(struct uLLUserInterface));
	return RES_OK;
}

void uLLUserInterfaceDistory(void)
{
	if(uUserInterface){
		free(uUserInterface);
	}

}

RES_Typedef uLLInit(void)
{
	unsigned int i=0;

	//地址池
	for(i=0; i<ADDR_POOL_LEN; i++)
	{
		gAddrPool[i].addr = ADDR_POOL_START + i;
		gAddrPool[i]._is_vaild = true;
	}
	
	//接收BUF
	uLLSendBuf = (char *)calloc(1, DEV_DATA_LEN_MAX);
	uLLRecvBuf = (char *)calloc(1, DEV_DATA_LEN_MAX);
	if((!uLLSendBuf) || (!uLLRecvBuf))
		return RES_LL_MEMORY_FAILED;

	if(uUserInterface){
		if(uUserInterface->uLLDeviceInit){
			if(!uUserInterface->uLLDeviceInit())
				return RES_LL_USER_DEFINED_FAILED;
		}
	}	
	return RES_OK;
}

void uLLDistory(void)
{
	if(uLLSendBuf)
		free(uLLSendBuf);
	if(uLLRecvBuf)
		free(uLLRecvBuf);
}

void uLLDelayMs(size_t t)
{
	uassert(uUserInterface);
	uassert(uUserInterface->uLLDeviceDelayMs);

	uUserInterface->uLLDeviceDelayMs(t);
}

RES_Typedef uLLFrameSend(const struct FRAME_DAT *frame)
{
	uassert(uUserInterface);
	uassert(uUserInterface->uLLDeviceSend);

	memset(uLLSendBuf,0,sizeof(uLLSendBuf));
	if(_uLLFramePacket(frame, uLLSendBuf) != RES_OK)
		return RES_LL_PRAMAS_INVALD;

	if(!uUserInterface->uLLDeviceSend(uLLSendBuf, frame->len + 8))
		return RES_LL_USER_DEFINED_FAILED;

	return RES_OK;
}

RES_Typedef uLLFrameRecv(struct FRAME_DAT *frame, 
								unsigned short timeout )
{
	size_t data_len = 0;
	RES_Typedef res;
	
	uassert(uUserInterface);
	uassert(uUserInterface->uLLDeviceRecv);

	data_len = uUserInterface->uLLDeviceRecv(uLLRecvBuf, timeout);
	if(!data_len)
		return RES_LL_USER_DEFINED_FAILED;

	res = _uLLFrameParse( frame, uLLRecvBuf, data_len - 2);
	if((!frame)||(res != RES_OK))
		return RES_LL_PRASE_FAILED;
	
	return RES_OK;
}





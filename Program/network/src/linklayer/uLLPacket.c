

#include "uLLPacket.h"





RES_Typedef uLLFrameSendEx(const MTYPE_Typedef mtype,
										const SUBTYPE_Typedef subtype,
										const unsigned int addr,
										const bool hd,
										const unsigned int len,
									 	char *data)
{
	struct FRAME_DAT frame_data;
	
	frame_data.mtype = mtype;
	frame_data.subtype = subtype;
	frame_data.addr = addr;
	frame_data.hd = hd;

	frame_data.data = frame_data.hd ? data : NULL;
	frame_data.len = frame_data.hd ? len : 0;

	return uLLFrameSend(&frame_data);
}


SUBTYPE_Typedef uLLFrameWait(const unsigned int retry)
{
	unsigned int t=0;
	struct FRAME_DAT fra_wt;

	if(!retry)
		return SUBTYPE_UNDEFINED;
	
	do{
		if(uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT) == RES_OK)
		{
			return fra_wt.subtype;
		}
	
		t++;
	}
	while(t<retry);

	return SUBTYPE_UNDEFINED;
}

SUBTYPE_Typedef uLLFrameWaitInAddr(const unsigned int addr,
											const unsigned int retry)
{
	unsigned int t=0;
	struct FRAME_DAT fra_wt;

	if(!retry)
		return SUBTYPE_UNDEFINED;
	
	do{
		if(uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT) == RES_OK)
		{
			if(fra_wt.addr == addr)
				return fra_wt.subtype;
		}
	
		t++;
	}
	while(t<retry);

	return SUBTYPE_UNDEFINED;
}




RES_Typedef uLLFrameWaitEx(const MTYPE_Typedef mtype,
							const SUBTYPE_Typedef subtype,
							const unsigned int addr,
							const unsigned int retry)
{
	unsigned int t=0;
	RES_Typedef res;
	struct FRAME_DAT fra_wt;

	if(!retry)
		return RES_LL_PRAMAS_INVALD;
	do{
		
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
		if((res == RES_OK)&&
			(fra_wt.mtype == mtype)&&
				(fra_wt.subtype == subtype)&&
					(fra_wt.addr == addr))
		{
			return RES_OK;
		}
	
		t++;
	}
	while(t<retry);

	return RES_LL_TIMEOUT;
}



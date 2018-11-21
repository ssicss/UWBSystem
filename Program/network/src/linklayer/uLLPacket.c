

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





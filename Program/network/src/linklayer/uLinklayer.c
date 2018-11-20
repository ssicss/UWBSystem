
#include "uLinklayer.h"

#include <stdio.h>


unsigned char *uLLRecvBuf;

static RES_Typedef uLLFrameParse(struct FRAME_DAT *obj, 
									 unsigned char *buf, 
									  const unsigned int len)
{
	unsigned char mtype = 0;
	unsigned char subtype = 0;

	if((buf[0]!=FRAME_INDENTIFICATION_BIT0)||(buf[1]!=FRAME_INDENTIFICATION_BIT1))
		return RES_RARSE_FAILED;
	

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
		return RES_RARSE_FAILED;
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


static RES_Typedef uLLFramePacket(const struct FRAME_DAT *obj,
									 unsigned char *buf)
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

RES_Typedef uLLInit(void)
{

	unsigned int i=0;

	//地址池
	for(i=0; i<ADDR_POOL_LEN; i++)
	{
		gAddrPool[i].addr = ADDR_POOL_PREFIX + i;
		gAddrPool[i]._is_vaild = true;
	}
	
	//接收BUF
	uLLRecvBuf = (unsigned char *)malloc(DEV_DATA_LEN_MAX);
	return uLLRecvBuf ? RES_OK : RES_MALLOC_FAILED;
}

void uLLDistory(void)
{
	free(uLLRecvBuf);
}

__inline void uLLDelayMs(const unsigned int t)
{
	delay_ms(t);
}


unsigned char frame_buf[128];
RES_Typedef uLLFrameSend(const struct FRAME_DAT *frame)
{
	unsigned char *buf_t = NULL;
	unsigned int len = 0;
	unsigned int status = 0;
	unsigned int t = 0;


	
	memset(frame_buf,0,sizeof(frame_buf));
	
	if(uLLFramePacket(frame, frame_buf) != RES_OK)
		return RES_PRAMAS_INVALD;

	len = frame->len + 8;


	//清标志位
	DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_TXFRS|
										  SYS_STATUS_TXPHS|
											SYS_STATUS_TXPRS|
											SYS_STATUS_TXFRB);

	//发送
	dwt_writetxdata(len+2, frame_buf, 0);
	dwt_writetxfctrl(len+2, 0);

	
	
	dwt_starttx(DWT_START_TX_IMMEDIATE);   //立即发送，延时发送？发送后打开接收器？ 暂未完善

	//等待发送完成
	do {
		status = DW1000_readData32(SYS_STATUS_ID, 0);
		t++;
	} while((!(status & (SYS_STATUS_TXFRS)))&&(t<100));
	if(t>=100)
	{
		SEGGER_RTT_printf(0, "send timeout\r\n");
		return RES_RETRY_FAILED;
	}

	return RES_OK;
}




RES_Typedef uLLFrameRecv(struct FRAME_DAT *frame, 
								unsigned short timeout )
{
	unsigned int status = 0;
	unsigned int data_len = 0;
	RES_Typedef res;

	dwt_setrxtimeout(timeout);
	dwt_rxenable(0);
	do {
		status = DW1000_readData32(SYS_STATUS_ID, 0);
	} while(!(status & (SYS_STATUS_RXFCG  |SYS_STATUS_ALL_RX_ERR)));

	//SYS_STATUS_RXFCG   帧CRC校验成功
	//SYS_STATUS_ALL_RX_ERR  接收时所有可能的出错情况

	if(!(status & SYS_STATUS_RXFCG))
	{
	    DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_ALL_RX_ERR);
	    dwt_rxreset();
		return RES_RECV_FLAG_FAILED;
	}
		
	DW1000_writeData32(SYS_STATUS_ID, 0, SYS_STATUS_RXFCG);
 
	data_len = DW1000_readData32(RX_FINFO_ID, 0) & RX_FINFO_RXFL_MASK_1023;
	if(data_len > DEV_DATA_LEN_MAX){
		return RES_RECV_DATA_TOOLARGE;
	}
		

	dwt_readrxdata(uLLRecvBuf, data_len, 0);

	res = uLLFrameParse( frame, uLLRecvBuf, data_len - 2);
	if((!frame)||(res != RES_OK))
		return RES_RARSE_FAILED;
	
	return RES_OK;
}





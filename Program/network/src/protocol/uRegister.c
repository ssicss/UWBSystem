
#include "uRegister.h"
#include "stdplus.h"
#include "stm32f4_delay.h"

#include <stdio.h>

struct DevAddrPool gAddrPool[ADDR_POOL_LEN];


static RES_Typedef _uPTPacketSend(const MTYPE_Typedef mtype,
									const SUBTYPE_Typedef subtype,
									const unsigned int addr,
									const bool hd,
									const unsigned int len,
									 unsigned char *data)
{
	struct FRAME_DAT fra_wrp;
	
	fra_wrp.mtype = mtype;
	fra_wrp.subtype = subtype;
	fra_wrp.addr = addr;
	fra_wrp.hd = hd;
	fra_wrp.len = 0;
	fra_wrp.data = NULL;
	
	if(fra_wrp.hd){
		fra_wrp.data = data;
		fra_wrp.len = len;
	}
		
	return uLLFrameSend(&fra_wrp);
}


static unsigned int ___uPTDeviceAddrAlloc(void)
{
	unsigned int i=0;

	for(i=0; i<ADDR_POOL_LEN; i++)
	{
		if(gAddrPool[i]._is_vaild)
		{
			gAddrPool[i]._is_vaild = false;
			return gAddrPool[i].addr;
		}
	}
	return 0;
}

static RES_Typedef __uPTSaveRandomAddr(const struct FRAME_DAT *obj, 
												unsigned int *addr_buf,
												unsigned int pt)
{
	if(!obj->hd)
		return RES_PACKET_ERROR;

	addr_buf[pt] = obj->data[0]&0xffff;
	return RES_OK;
}

struct FRAME_DAT fra_wt;

static RES_Typedef _uPTWaitDeviceRegister(unsigned int *addr_buf, 
													unsigned int *alloc_addr_buf,
													struct list *catch_device)
{
	
	RES_Typedef res;
	unsigned int i=0;
	unsigned int j=0;
	unsigned int t=0;
	bool _is_catch = false;
	
	memset(&fra_wt, 0, sizeof(struct FRAME_DAT));
	memset(addr_buf, 0, sizeof(addr_buf));
	//登记随机因子

	i=0;
	t=0;

	SEGGER_RTT_printf(0,"[1]");

	do{
		t++;

		SEGGER_RTT_printf(0,"_", t);
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
		
		if(res == RES_OK){
			SEGGER_RTT_printf(0,"|", t);
			__uPTSaveRandomAddr(&fra_wt, addr_buf, i++);
		}

	}while((i<SAVE_RANDOM_ADDR_MAX)&&(t<WAITDEV_REGISTER_RETRY));
	SEGGER_RTT_printf(0,"\n\r");

	//分配地址
	for(j=0; j<i; j++)
	{	
		if(j==0)
			SEGGER_RTT_printf(0,"[2]");
		alloc_addr_buf[j] = ___uPTDeviceAddrAlloc();
		if(alloc_addr_buf[j]){
			
			_uPTPacketSend(MTYPE_NT, SUBTYPE_ADDR_ALLOC_REQUEST, addr_buf[j], true, 4, (unsigned char *)&alloc_addr_buf[j]);
			//得到分配地址请求响应
			t=0;
			_is_catch = false;
			do{
				t++;
				res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
				if((res == RES_OK)&&
					((fra_wt.mtype == MTYPE_NT)&&(fra_wt.subtype == SUBTYPE_ADDR_ALLOC_RESPONS)))
				{
					_is_catch = true;
				}
			}
			while((t<100)&&(_is_catch==false));
			if(t>=100){
				SEGGER_RTT_printf(0,"x");
			}else{
				SEGGER_RTT_printf(0,"v");
				listnode_add(catch_device, &alloc_addr_buf[j]);
			}
	
		}else{
			SEGGER_RTT_printf(0,"\n\ralloc addr error\n\r");
		}


		if(j == i-1)
			SEGGER_RTT_printf(0,"\n\r");
	}

	

	return RES_OK;
}

static RES_Typedef _uPTWaitApRequest(SUBTYPE_Typedef request)
{
	RES_Typedef res;
	
	res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
	if(res == RES_OK){
		if((fra_wt.mtype == MTYPE_NT)&&
				(fra_wt.subtype == request))
		{
			return RES_OK;
		}else{
			return RES_RARSE_FAILED;
		}
	}
	else{
		return res;
	}

}


//等待接收超时退出，用来延时，但是延时时不能收到其它UWB数据，会造成提前退出
static void _uPTWait(const unsigned int t)
{
	unsigned int i=0;

	for(i=0; i<t; i++)
	{
		uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT*2/3);
	}
	
	
}

static RES_Typedef _uPTAllocRespons(int random)
{
	RES_Typedef res;
	struct MANAGER_DEV_INFO *dev=NULL;
	unsigned int t = 0;
	bool _is_catch = false;
	
	do{
		t++;
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
		if((res == RES_OK)&&
			((fra_wt.mtype == MTYPE_NT)&&(fra_wt.subtype == SUBTYPE_ADDR_ALLOC_REQUEST)) && ( fra_wt.addr == random))
		{
			_is_catch = true;
		}
	}
	while((t<100)&&(_is_catch==false));
	if(t>=100){
		return RES_TIMEOUT;
	}

	//分配请求包
	dev = (struct MANAGER_DEV_INFO *)listnode_head(guSVManagerDevice);
	if(!dev)
		return RES_SYS_NOINIT;


	dev->ip = *(unsigned int *)fra_wt.data;
	_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_ADDR_ALLOC_RESPONS, 0, false, 0, NULL));
	SEGGER_RTT_printf(0,"ip:%x\n\r", dev->ip);

	return RES_OK;


}
static RES_Typedef _uPTSigninRespons(const unsigned int addr)
{
	
	RES_Typedef res;
	struct MANAGER_DEV_INFO *dev=NULL;
	unsigned int t = 0;


	do{
		t++;
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
	}
	while((t<100)&&(res!=RES_OK));
	if(t>=100){
		return RES_TIMEOUT;
	}

	if((fra_wt.mtype == MTYPE_NT)&&(fra_wt.subtype == SUBTYPE_SIGNIN_RESPONS)){

		dev = (struct MANAGER_DEV_INFO *)malloc(sizeof(struct MANAGER_DEV_INFO));
		dev->ip = addr;
		dev->role = DEV_ROLE_LABEL;
		dev->nrole = DEV_NROLE_STA;
		dev->sn = 0;
		dev->priority = 0;
		listnode_add(guSVManagerDevice, dev);

		return RES_OK;
	}

	return RES_PACKET_ERROR;

}



RES_Typedef uPTRegisterCycle(void)
{

	unsigned int device_random_addr[SAVE_RANDOM_ADDR_MAX];
	unsigned int device_alloc_addr[SAVE_RANDOM_ADDR_MAX];
	struct list *catch_dev = NULL;
	struct listnode *catch_node = NULL;

	unsigned int i=0;
	RES_Typedef res;
	unsigned int t=0;
	
	char str_ip[16];
	unsigned int addr=0;

	
	catch_dev = list_new();
	if(!catch_dev)
		return RES_MALLOC_FAILED;
	memset(device_random_addr, 0, sizeof(device_random_addr));

	//广播注册包
	_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_REGISTER_REQUEST, 0, false, 0, 0));

	//接收响应，分配地址-
	_PCK(_uPTWaitDeviceRegister(device_random_addr, device_alloc_addr, catch_dev));

	//签到
	catch_node = catch_dev->head;
	for(i=0; i<catch_dev->count; i++)
	{
		if(i==0)
			SEGGER_RTT_printf(0,"[3]");
		
		inet_ntoa(str_ip, *((unsigned int *)catch_node->data) );
		addr = *((unsigned int *)catch_node->data);

		do{
			_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_SIGNIN_REQUEST, addr , false, 0, 0));
			res = _uPTSigninRespons(addr); 
			t++;
		}while((res != RES_OK)&&(t<5));

		
		
		if(t>=5)
		{
			SEGGER_RTT_printf(0,"x");
		}else{
			SEGGER_RTT_printf(0,"v");
		}

		if(i == catch_dev->count-1)
			SEGGER_RTT_printf(0,"\n\r");
		
		catch_node = catch_node->next;
	}
	
	list_free(catch_dev);
	return RES_OK;
}




RES_Typedef uPTRegisterRespons(void)
{
	int random = 0;
	float random_max = 0.0f;

	
	if(_uPTWaitApRequest(SUBTYPE_REGISTER_REQUEST) == RES_OK){
		

		//产生随机数
		srand(uwTick);
		random_max = WAITDEV_REGISTER_RETRY;
		random = (int)(random_max*rand()/(RAND_MAX+1.0));

		_uPTWait(random);

		_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_REGISTER_RESPONS, 0, true, 4, (unsigned char *)&random));
		_PCK(_uPTAllocRespons(random));

		
	
	}

	return RES_OK;
}


RES_Typedef uPTSigninRespons(void)
{
	if(_uPTWaitApRequest(SUBTYPE_SIGNIN_REQUEST) == RES_OK){
		_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_SIGNIN_RESPONS, 0, false, 0, NULL));
	}else{

	}

	return RES_OK;
}



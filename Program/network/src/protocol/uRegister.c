
#include "uRegister.h"
#include "stdplus.h"

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

	unsigned int out = 0;
	
	memset(&fra_wt, 0, sizeof(struct FRAME_DAT));
	memset(addr_buf, 0, sizeof(addr_buf));
	printf("scan devices");
	//登记随机因子
	do{
		t++;
		res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
		if(res == RES_OK){
			printf("^");
			__uPTSaveRandomAddr(&fra_wt, addr_buf, i++);
		}
		else{
			if(out > 10){
				out = 0;
				printf(".");
			}
				
			out++;	
		}
	}while((i<SAVE_RANDOM_ADDR_MAX)&&(t<WAITDEV_REGISTER_RETRY));

	//分配地址
	for(j=0; j<i; j++)
	{
		alloc_addr_buf[j] = ___uPTDeviceAddrAlloc();
		if(alloc_addr_buf[j]){
	
			//printf("\n\rcatch device[%s],alloc %s",inet_ntoa(addr_buf[j]), inet_ntoa(alloc_addr_buf[j]));
			_uPTPacketSend(MTYPE_NT, SUBTYPE_ADDR_ALLOC_REQUEST, addr_buf[j], true, 4, (unsigned char *)&alloc_addr_buf[j]);
			//得到分配地址请求响应
			res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
			if(res == RES_OK){
				printf("*");
				listnode_add(catch_device, &alloc_addr_buf[j]);
			}else{
				printf("x");
			}
			
		}else{
			printf("\n\ralloc addr error");
		}
			
	}

	printf("\n\r");

	return RES_OK;
}

static RES_Typedef _uPTWaitApRegisterRequest(void)
{
	RES_Typedef res;
	
	res = uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT);
	if(res == RES_OK){
		if((fra_wt.mtype == MTYPE_NT)&&
				(fra_wt.subtype == SUBTYPE_REGISTER_REQUEST))
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
	uLLFrameRecv(&fra_wt, WAITDEV_REGISTER_TIMEOUT/50*t);
}




RES_Typedef uPTRegisterCycle(void)
{
	unsigned int device_random_addr[SAVE_RANDOM_ADDR_MAX];
	unsigned int device_alloc_addr[SAVE_RANDOM_ADDR_MAX];
	struct list *catch_dev = NULL;
	unsigned int i=0;
	struct listnode *node = NULL;
	
	catch_dev = list_new();
	if(!catch_dev)
		return RES_MALLOC_FAILED;
	memset(device_random_addr, 0, sizeof(device_random_addr));

	//广播注册包
	_PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_REGISTER_REQUEST, 0, false, 0, 0));
	//接收响应，分配地址
	_PCK(_uPTWaitDeviceRegister(device_random_addr, device_alloc_addr, catch_dev));

	//签到
	node = catch_dev->head;
	for(i=0; i<catch_dev->count; i++)
	{
		
		printf("catch device[%s]\n\r", inet_ntoa( *((unsigned int *)node->data) ));
		node = node->next;
	}

	list_free(catch_dev);

	return RES_OK;
}


RES_Typedef uPTRegisterRespons(void)
{
	int random = 0;
	float random_max = 0.0f;
	struct MANAGER_DEV_INFO *dev=NULL;
	
	if(_uPTWaitApRegisterRequest() == RES_OK){

		//产生随机数
		random_max = WAITDEV_REGISTER_RETRY;
		random = (int)(random_max*rand()/(RAND_MAX+1.0));
		random = (int)(random_max*rand()/(RAND_MAX+1.0));
		//printf("recv .., random - %d\n\r", random);


		dev = (struct MANAGER_DEV_INFO *)listnode_head(guSVManagerDevice);
		if(!dev)
			return RES_SYS_NOINIT;
		
		if(!dev->ip)
		{
			//_uPTWait(random);
			//printf("");
		  _PCK(_uPTPacketSend(MTYPE_NT, SUBTYPE_REGISTER_RESPONS, 0, true, 4, (unsigned char *)&random));

		}
	
	}

	return RES_OK;
}




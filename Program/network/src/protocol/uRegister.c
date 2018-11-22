
#include "uRegister.h"


struct DevAddrPool gAddrPool[ADDR_POOL_LEN];

struct FRAME_DAT fra_wt;


static RES_Typedef _uPTSigninRespons(const unsigned int addr)
{
	struct MANAGER_DEV_INFO *dev=NULL;

	if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_SIGNIN_RESPONS, 0, 100) != RES_OK)
		return RES_PT_TIMEOUT;

	
	dev = (struct MANAGER_DEV_INFO *)malloc(sizeof(struct MANAGER_DEV_INFO));
	dev->ip = addr;
	dev->role = DEV_ROLE_LABEL;
	dev->nrole = DEV_NROLE_STA;
	dev->sn = 0;
	dev->priority = 0;
	listnode_add(guSVManagerDevice, dev);

	return RES_OK;


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


static RES_Typedef _uPTDeviceSignin(struct list * device_list)
{
	struct listnode *catch_node = NULL;

	unsigned int i=0;
	RES_Typedef res;
	unsigned int t=0;
	
	char str_ip[16];
	unsigned int addr=0;

	uassert(device_list);

	catch_node = device_list->head;
	for(i=0; i<device_list->count; i++)
	{
		if(i==0)
			SEGGER_RTT_printf(0,"[3]");
		
		inet_ntoa(str_ip, *((unsigned int *)catch_node->data) );
		addr = *((unsigned int *)catch_node->data);

		do{
			_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_SIGNIN_REQUEST, addr , false, 0, 0));
			res = _uPTSigninRespons(addr); 
			t++;
		}while((res != RES_OK)&&(t<5));

		if(t>=5)
		{
			SEGGER_RTT_printf(0,"x");
		}else{
			SEGGER_RTT_printf(0,"v");
		}

		if(i == device_list->count-1)
			SEGGER_RTT_printf(0,"\n\r");
		
		catch_node = catch_node->next;
	}
	
	return RES_OK;

}



static RES_Typedef _uPTWaitDeviceRegister(unsigned int *addr_buf, 
													unsigned int *alloc_addr_buf,
													struct list *catch_device)
{
	
	RES_Typedef res;
	unsigned int i=0;
	unsigned int j=0;
	unsigned int t=0;
	
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
			
			uLLFrameSendEx(MTYPE_NT, SUBTYPE_ADDR_ALLOC_REQUEST, addr_buf[j], true, 4, ( char *)&alloc_addr_buf[j]);
			//得到分配地址请求响应
			if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_ADDR_ALLOC_RESPONS, 0, 100) != RES_OK)
			{
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
	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_ADDR_ALLOC_RESPONS, 0, false, 0, NULL));
	SEGGER_RTT_printf(0,"ip:%x\n\r", dev->ip);

	return RES_OK;


}


RES_Typedef uPTRegisterCycle(void)
{

	unsigned int device_addr_save[SAVE_RANDOM_ADDR_MAX];
	unsigned int device_alloc_addr[SAVE_RANDOM_ADDR_MAX];
	struct list *catch_dev_list = NULL;


	catch_dev_list = list_new();
	if(!catch_dev_list)
		return RES_PT_MEMORY_FAILED;
	
	memset(device_addr_save, 0, sizeof(device_addr_save));

	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_REGISTER_REQUEST, 0, false, 0, 0));
	_PCK(_uPTWaitDeviceRegister(device_addr_save, device_alloc_addr, catch_dev_list));
	_PCK(_uPTDeviceSignin(catch_dev_list));

	list_free(catch_dev_list);
	return RES_OK;
}


RES_Typedef uPTReSignin(void)
{
	struct listnode *node = NULL;
	struct listnode *node_save = NULL;
	unsigned int i=0;
	struct MANAGER_DEV_INFO *dev;
	struct list *catch_dev_list = NULL;
	unsigned int node_count = 0;

	
	catch_dev_list = list_new();
	if(!catch_dev_list)
		return RES_PT_MEMORY_FAILED;

	node_count = guSVManagerDevice->count;

	node = guSVManagerDevice->head;
	for(i=0; i<node_count; i++)
	{
		if(i!=0){
			dev = (struct MANAGER_DEV_INFO *)node->data;
			SEGGER_RTT_printf(0,"add<%d> %x, sum:%d\n\r", i, dev->ip, node_count);
			listnode_add(catch_dev_list, &dev->ip);	
		}
		node = node->next;
	}



	node = guSVManagerDevice->tail;
	for(i=0; i<node_count-1; i++)
	{
		node_save = node;
		node = node->prev;
		SEGGER_RTT_printf(0,"delet %d, sum:%d\n\r", i, node_count);
		list_delete_node(guSVManagerDevice, node_save);
	}


	SEGGER_RTT_printf(0,"ready to signin %d\n\r", catch_dev_list->count);
	_PCK(_uPTDeviceSignin(catch_dev_list));

	list_free(catch_dev_list);
	
	return RES_OK;
}


RES_Typedef uPTRegisterRespons(void)
{
	int random = 0;
	float random_max = 0.0f;

	
	if(uLLFrameWaitEx(MTYPE_NT, SUBTYPE_REGISTER_REQUEST, 0, 100) == RES_OK){
		//产生随机数
		srand(uwTick);
		random_max = WAITDEV_REGISTER_RETRY;
		random = (int)(random_max*rand()/(RAND_MAX+1.0));

		_uPTWait(random);

		_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_REGISTER_RESPONS, 0, true, 4, ( char *)&random));
		_PCK(_uPTAllocRespons(random));
	}

	return RES_OK;
}


RES_Typedef uPTSigninRespons(void)
{
	_PCK(uLLFrameSendEx(MTYPE_NT, SUBTYPE_SIGNIN_RESPONS, 0, false, 0, NULL));
	return RES_OK;
}



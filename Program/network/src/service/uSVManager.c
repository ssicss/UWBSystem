

#include "uSVManager.h"
#include "stdplus.h"



struct USVMANAGER_CONTROL guSVManagerCtl;
struct list *guSVManagerDevice;




RES_Typedef uSVManagerInit(void)
{
	struct MANAGER_DEV_INFO *dev;

	memset(&guSVManagerCtl, 0, sizeof(struct USVMANAGER_CONTROL));
	guSVManagerDevice = list_new();
	if(!guSVManagerDevice)
		return RES_MALLOC_FAILED;

	dev = (struct MANAGER_DEV_INFO *)malloc(sizeof(struct MANAGER_DEV_INFO));
	if(!dev)
		return RES_MALLOC_FAILED;

	dev->ip = 0;
	dev->sn = 0xaaaa;
	dev->role = DEV_ROLE_BS;

#ifdef _DEV_NROLE_AP
	dev->nrole = DEV_NROLE_AP;
	printf("Login as AP\n\r#");
#else
	dev->nrole = DEV_NROLE_STA;
printf("Login as STA\n\r");

#endif


	dev->priority = 0;

	listnode_add(guSVManagerDevice, dev);
	
	
	return RES_OK;
}


RES_Typedef uSVManagerDiscoveryStart(void)
{

	guSVManagerCtl.flag |= (1<<0);
	return RES_OK;
}

RES_Typedef uSVManagerDiscoveryStop(void)
{
	
	guSVManagerCtl.flag &= (~(1<<0));

	return RES_OK;
}

RES_Typedef uSVManagerListDevice(void)
{
	

	return RES_OK;
}

RES_Typedef uSVManagerChageDevice(void)
{
	

	return RES_OK;
}

RES_Typedef uSVManagerNetCreate(void)
{
	

	return RES_OK;
}




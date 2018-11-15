#ifndef __USVMANAGER_H
#define __USVMANAGER_H

#include "uCommon.h"
#include "uRegister.h"

typedef enum{
	DEV_ROLE_BS = 0,
	DEV_ROLE_LABEL = 1,
}DEV_ROLE_Typedef;
	
typedef enum{
	DEV_NROLE_AP = 0,
	DEV_NROLE_STA = 1,
}DEV_NROLE_Typedef;


	
struct MANAGER_DEV_INFO{
	unsigned int ip;
	unsigned int sn;
	DEV_ROLE_Typedef role;
	DEV_NROLE_Typedef nrole;
	unsigned int priority;
};


struct USVMANAGER_CONTROL{
	unsigned int flag;

	unsigned int scan_count;
	unsigned int scan_count_cur;
};

extern struct USVMANAGER_CONTROL guSVManagerCtl;
extern struct list *guSVManagerDevice;


RES_Typedef uSVManagerInit(void);
RES_Typedef uSVManagerDiscoveryStart(void);
RES_Typedef uSVManagerDiscoveryStop(void);

#endif


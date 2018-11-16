
#include "uIdle.h"

#include "uSVManager.h"


RES_Typedef uSVIdle(void)
{
	struct MANAGER_DEV_INFO *dev=NULL;


	dev = (struct MANAGER_DEV_INFO *)listnode_head(guSVManagerDevice);
	if(!dev)
		return RES_SVNANAGER_SERVER_CANCEL;
	if(dev->nrole == DEV_NROLE_AP){
		
		//广播注册包
		if(guSVManagerCtl.flag & (1<<0))
		{
			if(!guSVManagerCtl.scan_count){
				if(uPTRegisterCycle())
					return RES_SVNANAGER_SERVER_CANCEL;
			}else{
				if(guSVManagerCtl.scan_count_cur<guSVManagerCtl.scan_count){
					if(uPTRegisterCycle())
						return RES_SVNANAGER_SERVER_CANCEL;
				}
				guSVManagerCtl.scan_count_cur++;
			}
		}



		//解析SHELL
		uSVShellPrase();
	}
	else if(dev->nrole == DEV_NROLE_STA){
		if(dev->ip){
			uPTSigninRespons();

			
		}else{
			uPTRegisterRespons();
		}
	

	}else{
		return RES_SVNANAGER_SERVER_CANCEL;
	}





	return RES_OK;
}





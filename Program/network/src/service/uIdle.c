
#include "uIdle.h"


extern struct USVMANAGER_CONTROL guSVManagerCtl;
extern struct list *guSVManagerDevice;

RES_Typedef uSVIdle(void)
{
	struct MANAGER_DEV_INFO *dev=NULL;
	SUBTYPE_Typedef packet_subtype = SUBTYPE_UNDEFINED;


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

		//发送包
		if(guSVManagerCtl.flag & (1<<1)){
			//uLLFrameSend(&guSVManagerCtl.readly_to_send);
		}

		//ping
		if(guSVManagerCtl.flag & (1<<2)){
			uPTPing(guSVManagerCtl.readly_to_send);

			guSVManagerCtl.flag &= (~(1<<2));
			if(guSVManagerCtl.readly_to_send){
				free(guSVManagerCtl.readly_to_send);
			}
		}

		
		//解析SHELL
		uSVShellPrase();
	}
	else if(dev->nrole == DEV_NROLE_STA){
		if(dev->ip){
			packet_subtype = uLLFrameWaitInAddr(dev->ip, 100);
			switch(packet_subtype)
			{
				case SUBTYPE_SIGNIN_REQUEST: 
							uPTSigninRespons();
							break;
				case SUBTYPE_PING_REQUEST:
							uPTPingRespons();
							break;
				default:break;
			}
		}else{
			uPTRegisterRespons();
		}
	

	}else{
		return RES_SVNANAGER_SERVER_CANCEL;
	}




	return RES_OK;
}





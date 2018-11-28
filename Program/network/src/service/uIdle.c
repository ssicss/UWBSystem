
#include "uIdle.h"


extern struct USVMANAGER_CONTROL guSVManagerCtl;
extern struct list *guSVManagerDevice;

RES_Typedef uSVIdle(void)
{
	struct MANAGER_DEV_INFO *dev=NULL;
	SUBTYPE_Typedef packet_subtype = SUBTYPE_UNDEFINED;
	char *parase_buf = NULL;


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

		//updata
		if(guSVManagerCtl.flag & (1<<1)){
			uPTGetConfigure();
			guSVManagerCtl.flag &= (~(1<<1));
		}

		//ping
		if(guSVManagerCtl.flag & (1<<2)){
			uPTPing(&guSVManagerCtl.readly_to_send);
			guSVManagerCtl.flag &= (~(1<<2));
		}

		//flush
		if(guSVManagerCtl.flag & (1<<3)){
			uPTReSignin();
			guSVManagerCtl.flag &= (~(1<<3));
		}

		//set
		if(guSVManagerCtl.flag & (1<<4)){
			uPTSetConfigure(&guSVManagerCtl.readly_to_send);
			guSVManagerCtl.flag &= (~(1<<4));
		}
		//start
		if(guSVManagerCtl.flag & (1<<5)){
			uPTRangingCyle(&guSVManagerCtl.readly_to_send);
			guSVManagerCtl.flag &= (~(1<<5));
		}
		//解析SHELL
		uSVShellPrase();
	}
	else if(dev->nrole == DEV_NROLE_STA){
		if(dev->ip){

			parase_buf = malloc(sizeof(struct MANAGER_DEV_INFO));
			if(!parase_buf)
				return RES_SVNANAGER_SERVER_CANCEL;
			
			packet_subtype = uLLFrameWaitInAddrEx(dev->ip, 300, parase_buf);
			switch(packet_subtype)
			{
				case SUBTYPE_SIGNIN_REQUEST: 
							uPTSigninRespons();
							break;
				case SUBTYPE_PING_REQUEST:
							uPTPingRespons();
							break;
				case SUBTYPE_GET_CONFIG_REQUEST:
							uPTGetConfigureRespons();
							break;
				case SUBTYPE_SET_CONFIG_REQUEST:
							uPTSetConfigureRespons(parase_buf);
							break;
				case SUBTYPE_RANGING_BEGIN_REQUEST:
							uPTRangingRespons(dev->ip, parase_buf);
							break;	
							
				default:break;	
			}

			if(parase_buf)
				free(parase_buf);
			
		}else{
			uPTRegisterRespons();
		}
	

	}else{
		return RES_SVNANAGER_SERVER_CANCEL;
	}




	return RES_OK;
}





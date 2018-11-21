

#include "uSVShell.h"


struct QUEUE *gUartQueue = NULL;

extern struct USVMANAGER_CONTROL guSVManagerCtl;
extern struct QUEUE *gUartQueue;
extern bool _is_uart_handle;
extern struct list *guSVManagerDevice;


RES_Typedef uSVShellInit(void)
{
	gUartQueue = QueueCreate(56);
	if(!gUartQueue)
		return RES_CREATE_QUEUE_ERROR;

	printf( "\n\r"
			"Welcome to Uwb Cloud Service\n\r"
			"\n\r"
	);

	return RES_OK;
}

static void _uSVShellCmdHelp(void){
	printf( "\n\r"
			"=================================================================\n\r"
			"=========== >ls: list all devices                        ========\n\r"
			"=========== >scan [OPTION].. : scan devices              ========\n\r"
			"===========      -c contine scan devices,never return    ========\n\r"
			"===========      -n scan n times                         ========\n\r"
			"=========== >?/help: help info                           ========\n\r"
			"=========== >clear: clear screen                         ========\n\r"
			"================================================================\n\r"
	);
}

static void _uSVShellCmdScan(const char *parames){
	unsigned int count=0;

	if(!strncmp(parames, " -c", strlen(" -c"))){
		guSVManagerCtl.scan_count = 0;
		guSVManagerCtl.flag |= 0x1;
	}
	else if(!strncmp(parames, " -n", strlen(" -n"))){
		count = atoi(&parames[strlen(" -n")+1]);

		if(count){
			guSVManagerCtl.scan_count_cur = 0;
			guSVManagerCtl.scan_count = count;
			guSVManagerCtl.flag |= 0x1;

		}else{
			printf("\n\rscan:times can not equel zero.\n\r");
		}	
	}
	else{
		printf("\n\rscan:undefined parames.\n\r");

	}
}

static void _uSVShellCmdEndProcess(void){
	guSVManagerCtl.flag &= 0xfe;
}


static void _uSVShellCmdLs(void){
	struct listnode *node = NULL;
	unsigned int i=0;
	struct MANAGER_DEV_INFO *dev;
	char str_role[5];
	char str_root[5];
	char str_ip[16];

	printf("\n\r       <ip>            <sn>       <role>     <nrole>     <priority>   \n\r");

	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		dev = (struct MANAGER_DEV_INFO *)node->data;

		memset(str_role, 0, sizeof(str_role));
		if(dev->role == DEV_ROLE_BS){
			strcat(str_role, "BS");
		}
		else if(dev->role == DEV_ROLE_LABEL){
			strcat(str_role, "LABEL");
		}

		memset(str_root, 0, sizeof(str_root));
		if(dev->nrole == DEV_NROLE_AP){
			strcat(str_root, "AP");
		}
		else if(dev->nrole == DEV_NROLE_STA){
			strcat(str_root, "STA");
		}
		
		inet_ntoa(str_ip, dev->ip);
		
		
		printf("(%d) %-15s  %08x        %-s        %-s            %-d\n\r", 
					i, 
					str_ip,
					dev->sn,
					str_role,
					str_root,
					dev->priority);
		node = node->next;
	}
}


static void _uSVShellCmdUpdata(const char *parames)
{
	struct listnode *node = NULL;
	unsigned int i=0;
	struct MANAGER_DEV_INFO *dev;
	
	node = guSVManagerDevice->head;
	for(i=0; i<guSVManagerDevice->count; i++)
	{
		if(i>0){
			dev = (struct MANAGER_DEV_INFO *)node->data;
			printf("addr=%x\n\r", dev->ip);

			//guSVManagerCtl.readly_to_send.addr = dev->ip;
		}
		node = node->next;
	}


	


	//guSVManagerCtl.flag |= (1<<1);
	
	
	
}

static void _uSVShellCmdPing(const char *parames)
{

	guSVManagerCtl.readly_to_send = (struct FRAME_DAT *)malloc(sizeof(struct FRAME_DAT));
	if(!guSVManagerCtl.readly_to_send)
		return;

	guSVManagerCtl.readly_to_send->addr = inet_aton(parames);
	guSVManagerCtl.readly_to_send->mtype = MTYPE_NT;
	guSVManagerCtl.readly_to_send->subtype = SUBTYPE_PING_REQUEST;
	guSVManagerCtl.readly_to_send->hd = false;
	guSVManagerCtl.readly_to_send->data = NULL;
	guSVManagerCtl.readly_to_send->len = 0;

	if(guSVManagerCtl.readly_to_send->addr){
		guSVManagerCtl.flag |= (1<<2);
	}else{
		printf("\n\rundefined host addr");
	}
}

RES_Typedef uSVShellPrase(void)
{
	unsigned int data=0;
	 char str_cmd[128];

	char str_tmp[2];

	if(_is_uart_handle == true)
	{
		_is_uart_handle = false;
		memset(str_cmd, 0, sizeof(str_cmd));

		while(QueuePop(gUartQueue, &data))
		{
			str_tmp[0] = ( char)data;
			str_tmp[1] = '\0';
			strcat(str_cmd, str_tmp);
		}
		
		printf("#");
		
		
		if((strcmp(str_cmd, "?") == 0) || (strcmp(str_cmd, "help") ==0)){
			_uSVShellCmdHelp();
		}
		else if((strncmp(str_cmd, "scan", strlen("scan")) == 0)){
			_uSVShellCmdScan(&str_cmd[strlen("scan")]);
		}
		else if((strncmp(str_cmd, "ls", strlen("ls")) == 0)){
			_uSVShellCmdLs();
		}
		else if((strncmp(str_cmd, "updata", strlen("updata")) == 0)){
			_uSVShellCmdUpdata(&str_cmd[strlen("updata")]);
		}
		else if((strncmp(str_cmd, "ping", strlen("ping")) == 0)){
			_uSVShellCmdPing(&str_cmd[strlen("ping")]);
		}

		
		else if(str_cmd[0] == 0x3){
			//ctrl+c
			_uSVShellCmdEndProcess();
		}
		else if((strcmp(str_cmd, "") == 0)){
			
		}
		else if(strcmp(str_cmd, "clear") == 0){
			printf("\x0c");;
		}
		else{
			printf("\n\rundefined, press \"?\" get help.");
		}

		printf("\n\r#");
		
	}
	
	
	

	return RES_OK;
}





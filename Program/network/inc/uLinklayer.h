#ifndef __ULINKLAYER_H
#define __ULINKLAYER_H

#include "uCommon.h"
#include "uRegister.h"

#define FRAME_INDENTIFICATION_BIT0  'W'
#define FRAME_INDENTIFICATION_BIT1  'K'

#define DEV_DATA_LEN_MAX (1024)


typedef enum{
	MTYPE_NT = 0,
	MTYPE_RANGING = 1,
}MTYPE_Typedef;

typedef enum{
	SUBTYPE_REGISTER_REQUEST = 1,
	SUBTYPE_REGISTER_RESPONS = 2,
	SUBTYPE_ADDR_ALLOC_REQUEST = 3,
	SUBTYPE_ADDR_ALLOC_RESPONS = 4,
	SUBTYPE_GET_CONFIG_REQUEST = 5,
	SUBTYPE_GET_CONFIG_RESPONS = 6,
	SUBTYPE_SIGNIN_REQUEST = 7,
	SUBTYPE_SIGNIN_RESPONS = 8,
	SUBTYPE_ADDR_RELEASE_REQUEST = 9,
	SUBTYPE_ADDR_RELEASE_RESPONS = 10,
	SUBTYPE_RANGING_REQUEST = 11,
	SUBTYPE_RANGING_RESPONS = 12,

	
	SUBTYPE_POLL = 1,
	SUBTYPE_RESPONS = 2,
	SUBTYPE_FINAL =3,
}SUBTYPE_Typedef;


struct FRAME_DAT{
	MTYPE_Typedef mtype;
	bool hd;
	SUBTYPE_Typedef subtype;
	unsigned int addr;

	unsigned int len;
	unsigned char *data;
};


RES_Typedef uLLInit(void);
void uLLDistory(void);
RES_Typedef uLLFrameSend(const struct FRAME_DAT *frame);
RES_Typedef uLLFrameRecv(struct FRAME_DAT *frame, 
								unsigned short timeout );

__inline void uLLDelayMs(const unsigned int t);


#endif



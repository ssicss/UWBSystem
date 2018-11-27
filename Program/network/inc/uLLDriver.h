#ifndef __ULINKLAYER_H
#define __ULINKLAYER_H

#include "uCommon.h"

#define FRAME_INDENTIFICATION_BIT0  'W'
#define FRAME_INDENTIFICATION_BIT1  'K'

#define DEV_DATA_LEN_MAX (1024)


typedef enum{
	MTYPE_NT = 0,
	MTYPE_RANGING = 1,
}MTYPE_Typedef;

typedef enum{
	SUBTYPE_UNDEFINED = 0,
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
	SUBTYPE_PING_REQUEST = 13,
	SUBTYPE_PING_RESPONS = 14,
	SUBTYPE_SET_CONFIG_REQUEST = 15,
	SUBTYPE_SET_CONFIG_RESPONS = 16,
	SUBTYPE_RANGING_DATRETURN = 17,
	
	SUBTYPE_POLL = 1,
	SUBTYPE_RESPONS = 2,
	SUBTYPE_FINAL =3,
}SUBTYPE_Typedef;

struct uLLUserInterface{
	bool (*uLLDeviceInit)(void);
	bool (*uLLDeviceSend)(const char *, size_t len);
	bool (*uLLDeviceSendExpResp)(const char *, size_t len);
	size_t (*uLLDeviceRecv)(char *, size_t);
	size_t (*uLLDeviceRecvNoPre)(char *, size_t);
	void (*uLLDeviceDelayMs)(size_t);
	unsigned long long (*uLLDeviceGetRxTimestamp)(void);
	unsigned long long (*uLLDeviceGetTxTimestamp)(void);
	bool (*uLLDeviceSendDelayed)(const char *, size_t, unsigned int);
};

struct FRAME_DAT{
	MTYPE_Typedef mtype;
	bool hd;
	SUBTYPE_Typedef subtype;
	unsigned int addr;

	unsigned int len;
	char *data;
};


RES_Typedef uLLInit(void);
void uLLDistory(void);
RES_Typedef uLLFrameSend(const struct FRAME_DAT *frame);
RES_Typedef uLLFrameRecv(struct FRAME_DAT *frame, unsigned short timeout );
void uLLDelayMs(size_t t);


RES_Typedef uLLUserInterfaceRegister(struct uLLUserInterface *interface);
void uLLUserInterfaceDistory(void);
RES_Typedef uLLFrameSendExpResp(const struct FRAME_DAT *frame);
unsigned long long uLLGetRxTimeStamp(void);
RES_Typedef uLLFrameRecvNoPre(struct FRAME_DAT *frame, unsigned short timeout );
unsigned long long uLLGetTxTimeStamp(void);
RES_Typedef uLLFrameSendDelayed(const struct FRAME_DAT *frame, unsigned int delay);


#endif



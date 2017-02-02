/*!****************************************************************************
* @file    gsmTSK.h
* @author  4eef
* @version V1.0
* @date    19-Aug-16
* @brief   --
*/
#ifndef gsmTSK_H
#define gsmTSK_H

/*!****************************************************************************
* Include
*/
#include "uart.h"
#include "string.h"
#include "os_init.h"
#include "FreeRTOS.h"
#include "stdio.h"
#include "stdlib.h"
#include "pduenc.h"
#include "gpsTSK.h"

/*!****************************************************************************
* User define
*/

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* User typedef
*/
typedef struct{
    uint8_t     pduLen;
    uint8_t     pduType;
    uint8_t     sndrNumType;
    uint8_t     sndrNumLen;
    uint8_t     sndrNum[6];
    uint8_t     PID;
    uint8_t     DCS;
    uint32_t    unixTime;
    uint8_t     msgLength;
    uint8_t     message[160];
    uint8_t     PDU[200];
    uint8_t     charPDU[400];
    uint8_t     rxSmsNum[4];
    uint8_t     smsUnread;
    uint16_t    rcvdSmsCnt;
    uint8_t     rdngSms;
}rxSMS_type;

typedef struct{
    uint8_t     pduLen;
    uint8_t     txPduLen[4];
    uint8_t     pduType;
    uint8_t     msgRef;
    uint8_t     rcvrNumType;
    uint8_t     rcvrNumLen;
    uint8_t     rcvrNum[6];
    uint8_t     PID;
    uint8_t     DCS;
    uint8_t     VP;
    uint8_t     msgLength;
    uint8_t     message[160];
    uint8_t     PDU[200];
    uint8_t     charPDU[400];
    uint8_t     txSmsNum[4];
    uint8_t     sndngSms;
    uint8_t     smsUnsent;
    uint16_t    sentSmsCnt;
    uint16_t    txFailed;
}txSMS_type;

typedef enum{
    ansOk = 0,
    smsRcvd,
    getGPS,
    gpsOff,
    gpsNotRdy,
    gpsString,
    gpsOnCmd,
    gpsOffCmd,
    ok,
    readMsg,
    delReadMsgs,
    delSentMsgs,
    pduMode,
    sendMsg,
    wellcome,
    msgSent,
    incoming,
    cmsErr,
    txErr,
    discardCall,
    AT,
    getModelId,
    getFirmRev,
    getIMEI,
    getUnrLst,
    getUnsntLst,
    saveMsg,
    sendStMsg,
    msgStored,
    stMsgSent,
    delAllMsgs,
    delMsg,
    getReadLst
}strings_enum_type;

typedef struct{
    uint8_t     __reserv    :1;
    uint8_t     gsmSmsRcvd  :1;
    uint8_t     gsmSmsTxRdy :1;
    uint8_t     gsmCallMssd :1;
    uint8_t     gsmOk       :1;
    uint8_t     gsmOn       :1;
    uint8_t     gsmError    :1;    
    uint8_t     gsmSet      :1;
}gsmStatus_type;

typedef struct{
    char            firmRev[32];
    char            modelInfo[32];
    char            IMEI[16];
    gsmStatus_type  gsmStatus;
    rxSMS_type      rxSMS;
    txSMS_type      txSMS;
}gsmData_type;

/*!****************************************************************************
* Extern viriables
*/

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void gsmTSK(void *pPrm);
uint8_t sndSms(void);//uint8_t sndSms(char *str, uint8_t *num);
uint8_t rcvSms(void);
char * string(strings_enum_type txt);
char * fillGpsString(void);
uint8_t gsmSendString(char *dst, char *src, uint32_t tout);

#endif //gsmTSK_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

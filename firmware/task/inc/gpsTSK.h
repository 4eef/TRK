/*!****************************************************************************
* @file    gpsTSK.h
* @author  4eef
* @version V1.0
* @date    03-Aug-16
* @brief   --
*/
#ifndef gpsTSK_H
#define gpsTSK_H

/*!****************************************************************************
* Include
*/
#include "uart.h"
//#include "crc.h"
#include "string.h"
#include "os_init.h"
#include "FreeRTOS.h"
#include "stdio.h"
#include "stdlib.h"
#include "tmtos.h"

/*!****************************************************************************
* User define
*/
#define PieceBufRx          8

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* User typedef
*/
typedef struct{
    uint16_t    waitRxTime_ms;
    uint16_t    readBytes;
    uint16_t    packTxCnt;
    uint16_t    packRxCnt;
    uint16_t    packErrorCnt;
}uartTask_type;

typedef struct{
    uint32_t    normRequest;
    uint32_t    errorRequest;
}uartTsk_type;

//typedef struct{
//    char        *str;
//    uint8_t     len;
//}commands_type;

typedef enum{
    outFmtOff = 0,
    outFmt,
    hotStart,     
    warmStart,
    coldStart,
    fullColdStart,
    standBy,
    getFirmware,
    firmInfo
}gpsCommands_enum_type;

typedef struct{
    uint8_t     __reserv    :4;
    uint8_t     gpsOk       :1;
    uint8_t     gpsOn       :1;
    uint8_t     gpsError    :1;    
    uint8_t     gpsSet      :1;
}gpsStatus_type;

typedef struct{
    int             lat_degrees;
    int             lat_minutes;
    float           lat_seconds;
    char            lat_sign;
    int             lon_degrees;
    int             lon_minutes;
    float           lon_seconds;
    char            lon_sign;
    int             precision;
    int             numSats;
    float           HDOP;
    float           altitude;
    char            altUnit;
    float           speedKnots;
    float           speedKph;
    uint32_t        unixTime;
    char            moduleInfo[64];
    uint32_t        freq;
    gpsStatus_type  gpsStatus;
}gpsData_type;

/*!****************************************************************************
* Extern viriables
*/
extern char *rxbuff;

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void gpsTSK(void *pPrm);
//char *fgets(char *s, int n, FILE *f);
void divideStr(char GPGGA[], char GPVTG[], char GPZDA[], char rxtmp[]);

#endif //gpsTSK_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

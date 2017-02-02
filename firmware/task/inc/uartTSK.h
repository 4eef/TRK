/*!****************************************************************************
* @file    uartTSK.h 
* @author  D_EL
* @version V1.1
* @date    2015-20-11
* @brief
*/
#ifndef uartTSK_H
#define uartTSK_H

/*!****************************************************************************
* Include
*/
#include "uart.h"
#include "crc.h"
#include "string.h"
#include "os_init.h"

/*!****************************************************************************
* User define
*/
#define PieceBufRx          8

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

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* Extern viriables
*/

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void uartTSK(void *pPrm);

#endif //uartTSK_H
/************ (C) COPYRIGHT ************ END OF FILE ************ DEL ********/

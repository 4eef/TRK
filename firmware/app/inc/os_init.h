/*!****************************************************************************
* @file    os_init.h
* @author  4eef
* @version V1.0
* @date    28-Jul-16
* @brief   --
*/
#ifndef os_init_H
#define os_init_H

/*!****************************************************************************
* Include
*/
#include "FreeRTOS.h"
#include "blinkTSK.h"
#include "btnTSK.h"
#include "semphr.h"
#include "gpsTSK.h"
#include "gsmTSK.h"
#include "sysTSK.h"

/*!****************************************************************************
* User define
*/

/*!****************************************************************************
* Semaphore
*/
extern xSemaphoreHandle semUart1Tx;
extern xSemaphoreHandle semUart1Rx;
extern xSemaphoreHandle semUart2Tx;
extern xSemaphoreHandle semUart2Rx;
//extern xSemaphoreHandle semGPSrdy;
//extern xSemaphoreHandle semGpsMutex;

/*!****************************************************************************
*-------NAME--------------------size [4byte Word] */
//#define BLINK_TSK_SZ_STACK      128
//#define BTN_TSK_SZ_STACK        32
#define GPS_TSK_SZ_STACK        512
#define GSM_TSK_SZ_STACK        512
#define SYS_TSK_SZ_STACK        128

/*!****************************************************************************
*-------NAME--------------------priority */
//#define BLINK_TSK_PRIO          1
//#define BTN_TSK_PRIO            2
#define GPS_TSK_PRIO            2
#define GSM_TSK_PRIO            3
#define SYS_TSK_PRIO            1


/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* User typedef
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
void os_init(void);

#endif //os_init_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

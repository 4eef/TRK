/*!****************************************************************************
* @file    sysTSK.h
* @author  4eef
* @version V1.0
* @date    04-Oct-16
* @brief   --
*/
#ifndef sysTSK_H
#define sysTSK_H

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
#include "gsmTSK.h"

/*!****************************************************************************
* User define
*/

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
void sysTSK(void *pPrm);

#endif //sysTSK_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

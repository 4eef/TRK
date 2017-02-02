/*!****************************************************************************
* @file    pduenc.h
* @author  4eef
* @version V1.0
* @date    23-Aug-16
* @brief   --
*/
#ifndef pduenc_H
#define pduenc_H

/*!****************************************************************************
* Include
*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "os_init.h"
#include "gsmTSK.h"
#include "tmtos.h"
#include "math.h"

/*!****************************************************************************
* User define
*/
#define YEAR        2000

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
void gsmDefaultDecode(uint8_t *pdu, uint8_t *str, uint16_t len);
uint8_t swapNibbles(uint8_t x);
void pduDecode(void);
uint8_t bcd2int(uint8_t hex);
void pduEncode(void);
void gsmDefaultEncode(uint8_t *str, uint8_t *pdu, uint16_t len);
void pdu2string(void);
void string2pdu(void);
void sndrNumCmp(uint8_t *sendernum);
uint16_t int2bcd(uint8_t ival);
void hex2char(uint8_t *dst, uint8_t *src, uint8_t len);
void char2hex(uint8_t *dst, uint8_t *src, uint8_t len);
//uint8_t * rcvrNumCmp(uint8_t *chars);

#endif //pduenc_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

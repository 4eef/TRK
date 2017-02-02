/*!****************************************************************************
* @file    tmtos.h
* @author  4eef
* @version V1.0
* @date    16-Aug-16
* @brief   --
*/
#ifndef tmtos_H
#define tmtos_H

/*!****************************************************************************
* Include
*/
#include "tmtos.h"
#include "stm32l4xx.h"
#include "stdio.h"
#include "stdlib.h"

/*!****************************************************************************
* User define
*/

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* User typedef
*/
typedef	unsigned short			u16_t;
typedef	unsigned long			u32_t;
typedef	signed short			s16_t;
typedef	signed long			    s32_t;

// DEF: standard signed format
// UNDEF: non-standard unsigned format
//#define	_XT_SIGNED

#ifdef	_XT_SIGNED
    typedef	s32_t                           xtime_t;
#else
    typedef	u32_t                           xtime_t;
#endif

typedef struct{       /* date and time components */
    int     tm_sec;
    int 	tm_min;
    int	    tm_hour;
    int     tm_mday;
    int 	tm_mon;
    int 	tm_year;
}tm_type;

/*!****************************************************************************
* Extern viriables
*/

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void xttotm(xtime_t secs);
xtime_t xtmtot();

#endif //tmtos_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

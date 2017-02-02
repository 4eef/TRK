/*!****************************************************************************
* @file    tmtos.c
* @author  4eef
* @version V1.0
* @date    16-Aug-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "tmtos.h"

/*!****************************************************************************
* MEMORY
*/
#define _TBIAS_DAYS		((70 * (u32_t)365) + 17)
#define _TBIAS_SECS		(_TBIAS_DAYS * (xtime_t)86400)
#define	_TBIAS_YEAR		1900
#define MONTAB(year)		((((year) & 03) || ((year) == 0)) ? mos : lmos)

const s16_t	lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
const s16_t	mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define	Daysto32(year, mon)	(((year - 1) / 4) + MONTAB(year)[mon])

tm_type tm;

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
xtime_t xtmtot(){	/* convert time structure to scalar time */
s32_t		days;
xtime_t		secs;
s32_t		mon, year;

	/* Calculate number of days. */
	mon = tm.tm_mon - 1;
	year = tm.tm_year - _TBIAS_YEAR;
	days  = Daysto32(year, mon) - 1;
	days += 365 * year;
	days += tm.tm_mday;
	days -= _TBIAS_DAYS;

	/* Calculate number of seconds. */
	secs  = 3600 * tm.tm_hour;
	secs += 60 * tm.tm_min;
	secs += tm.tm_sec;

	secs += (days * (xtime_t)86400);

	return (secs);
}

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void xttotm(xtime_t secsarg){
u32_t		secs;
s32_t		days;
s32_t		mon;
s32_t		year;
s32_t		i;
const s16_t*	pm;
    
    #ifdef	_XT_SIGNED
        if (secsarg >= 0) {
            secs = (u32_t)secsarg;
            days = _TBIAS_DAYS;
        } else {
            secs = (u32_t)secsarg + _TBIAS_SECS;
            days = 0;
        }
    #else
        secs = secsarg;
        days = _TBIAS_DAYS;
    #endif
    
        /* days, hour, min, sec */
    days += secs / 86400;		secs = secs % 86400;
    tm.tm_hour = secs / 3600;	secs %= 3600;
    tm.tm_min = secs / 60;		tm.tm_sec = secs % 60;
    
        /* determine year */
    for (year = days / 365; days < (i = Daysto32(year, 0) + 365*year); ) { --year; }
    days -= i;
    tm.tm_year = year + _TBIAS_YEAR;
    
        /* determine month */
    pm = MONTAB(year);
    for (mon = 12; days < pm[--mon]; );
    tm.tm_mon = mon + 1;
    tm.tm_mday = days - pm[mon] + 1;
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

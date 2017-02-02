/*!****************************************************************************
* @file    key.h
* @author  d_el
* @version V1.0
* @date    27.04.2016, by d_el
* @brief   --
*/
#ifndef key_H
#define key_H
#include "gpio.h"
#include "stdio.h"
#include "string.h"

/*!****************************************************************************
* User include
*/

/*!****************************************************************************
* User define
*/
#define KEY_NUM         3
#define VKEY_NUM        3
#define KEY_SAMPLES     25

/*!****************************************************************************
* User typedef
*/
typedef struct{
    uint8_t     k1      :1;
    //uint8_t     k2      :1;
    //uint8_t     k3      :1;
}phiKeyStruct;

//typedef struct{
//    uint8_t     vk31    :1;
//    uint8_t     vk32    :1;
//    uint8_t     vk33    :1;
//}vKeyStruct;

typedef struct{
    uint8_t     k1      :1;
//    uint8_t     k2      :1;
//    uint8_t     k3      :1;
//    uint8_t     vk31    :1;
//    uint8_t     vk32    :1;
//    uint8_t     vk33    :1;
}keyStruct;

typedef struct{
    union{
        phiKeyStruct    key;
        uint8_t         all;
    }dInState;
    
    union{
        phiKeyStruct    key;
        uint8_t         all;
    }dInPrevState;
    
    uint8_t             cnt[KEY_NUM];

    uint8_t             vKeyRdy;
    
    union{
        keyStruct       key;
        uint8_t         all;
    }keyState;
}key_type;

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* Extern viriables
*/
extern key_type    key;;

/*!****************************************************************************
* Macro functions
*/

/*!****************************************************************************
* Prototypes for the functions
*/
void keyProc(void);
void keyDriverReset(void);

#endif //key_H
/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

/*!****************************************************************************
* @file    key.c
* @author  d_el
* @version V1.0
* @date    27.04.2016, by d_el
* @brief   --
*/
#include "key.h"

/*!****************************************************************************
* Memory
*/
key_type    key;

/*!****************************************************************************
* @brief
*/
void dInUpdate(key_type *pkey, uint32_t num, uint32_t val){
    if(val != 0){
        if(pkey->cnt[num] < KEY_SAMPLES){
            pkey->cnt[num]++;
        }else{
            pkey->dInState.all |= 1U << num;
        }
    }
    else{
        if(pkey->cnt[num] > 0){
            pkey->cnt[num]--;
        }else{
            pkey->dInState.all &= ~(1U << num);
        }
    }
}

/*!****************************************************************************
* @brief
* @param
* @retval
*/
void keyProc(void){
    key_type    *pkey = &key;   
    uint32_t    mask;
    
    /***********************************************
    * Физические кнопки
    */
    //Фильтруем и обновляем дискретные входы
    dInUpdate(pkey, 0, !gppin_get(GP_B1));

    //Детектируем фронт сигнала
    for(mask = 1 << 0; mask < (1 << KEY_NUM); mask <<= 1){
        if(((pkey->dInPrevState.all & mask) == 0)&&((pkey->dInState.all & mask) != 0)){
            pkey->keyState.all |= mask;
        }else{
            pkey->keyState.all &= ~mask;
        }
    }
    pkey->dInPrevState.all = pkey->dInState.all;
}

/*!****************************************************************************
* @brief
* @param
* @retval
*/
void keyDriverReset(void){
    memset(&key, 0, sizeof(key_type));
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** d_el ****/

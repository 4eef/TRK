/*!****************************************************************************
* @file    blinkTSK.c
* @author  4eef
* @version V1.0
* @date    28-Jul-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "blinkTSK.h"

/*!****************************************************************************
* MEMORY
*/
//uartTsk_type        uartTsk;

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void blinkTSK(void *pPrm){
    //uint16_t *uart1
    
    //uart_init(USART1, BR9600);

    while(1){
        /*keyProc();

        uart_read(uart1, uart1->pRxBff, 32);

        for (int i = 10; i<=41; i++){
            uart2->pTxBff [i-10] = i;
        }
        
        uart_write(uart2, uart2->pTxBff, 28);
        
        xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
        
        int8_t result = 0;
        
        //result = strcmp(uart2->pTxBff, uart1->pRxBff, 28);
        
        for(uint8_t i = 0; i < 28; i++){
            if(uart2->pTxBff[i] != uart1->pRxBff[i]){
                result = 1;
            }
        }
        
        if(result != 0){
            error++;
        }else{
            success++;
        }
            
        
        for (int i = 0; i<32; i++){
            uart1->pRxBff [i] = 0;
        }
        
        //if(key.dInState.key.k1 == 0){
            gppin_set(GP_Debug_LED);
            vTaskDelay(pdMS_TO_TICKS(500));
            gppin_reset(GP_Debug_LED);
            vTaskDelay(pdMS_TO_TICKS(500));
        //}   */
//    vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

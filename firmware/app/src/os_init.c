/*!****************************************************************************
* @file    os_init.c
* @author  4eef
* @version V1.0
* @date    28-Jul-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "os_init.h"

/*!****************************************************************************
* Semaphore
*/
xSemaphoreHandle semUart1Tx;
xSemaphoreHandle semUart1Rx;
xSemaphoreHandle semUart2Tx;
xSemaphoreHandle semUart2Rx;
//xSemaphoreHandle semGPSrdy;
xSemaphoreHandle semGpsMutex;

/*!****************************************************************************
* MEMORY
*/

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void os_init(void){
    BaseType_t Result;

    //UART semaphores
    vSemaphoreCreateBinary(semUart1Tx);
    xSemaphoreTake(semUart1Tx, portMAX_DELAY);
    vSemaphoreCreateBinary(semUart1Rx);
    xSemaphoreTake(semUart1Rx, portMAX_DELAY);
    vSemaphoreCreateBinary(semUart2Tx);
    xSemaphoreTake(semUart2Tx, portMAX_DELAY);
    vSemaphoreCreateBinary(semUart2Rx);
    xSemaphoreTake(semUart2Rx, portMAX_DELAY);
    
    Result = pdTRUE;
    
//    Result &= xTaskCreate(blinkTSK, "blinkTSK", BLINK_TSK_SZ_STACK, NULL, BLINK_TSK_PRIO, NULL);
    Result &= xTaskCreate(gpsTSK, "gpsTSK", GPS_TSK_SZ_STACK, NULL, GPS_TSK_PRIO, NULL);
    Result &= xTaskCreate(gsmTSK, "gsmTSK", GSM_TSK_SZ_STACK, NULL, GSM_TSK_PRIO, NULL);
    Result &= xTaskCreate(sysTSK, "sysTSK", SYS_TSK_SZ_STACK, NULL, SYS_TSK_PRIO, NULL);
//    Result &= xTaskCreate(btnTSK, "btnTSK", BTN_TSK_SZ_STACK, NULL, BTN_TSK_PRIO, NULL);
    
    if(Result == pdTRUE){
    }
    else{
        while(1);
    }
    
    vTaskStartScheduler();

}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

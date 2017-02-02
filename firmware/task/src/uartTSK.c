/*!****************************************************************************
* @file    uartTSK.c 
* @author  D_EL
* @version V1.1
* @date    2015-20-11
* @brief
*/

/*!****************************************************************************
* Include
*/
#include "uartTSK.h"

/*!****************************************************************************
* MEMORY
*/
uartTsk_type        uartTsk;

/*!****************************************************************************
* @brief	Задача обслуживащая uart и ModBus
*/
uint8_t dum = 0xAA;
void uartTSK(void *pPrm){
    BaseType_t  res;
    uint16_t    crc;
    uint8_t     numRx;
    
    numRx = 0;
	//ask the firmware version, and if it is OK - set "1" in the struct
	//if not - delete TSK from manager
	
	//set the packet format
    
    while(1){
        res = xSemaphoreTake(uartRxSem, pdMS_TO_TICKS(5));
        numRx += PieceBufRx - uartGetRemainRx(uart3);
        
        if(numRx != 0){
            if(res == pdTRUE){  //Значит прочитали часть данных
                uart_read(uart3, uart3->pRxBff + numRx, PieceBufRx);
            }else{  //Прием закончен
                //uart_stopRead(uart3);
                crc = GetCrc(uart3->pRxBff, sizeof(task_type) + sizeof(uint16_t));
                if(crc == 0){
                    //Вынимаем принятые данные
                    memcpy(&rg.tf.task, uart3->pRxBff, sizeof(task_type));
                    xSemaphoreGive(rxRequest);  //Отдаем семафор
                    uartTsk.normRequest++;
                    //numRx = 0;
                    
                    //Отправка
                    memcpy(uart3->pTxBff, &rg.tf.state, sizeof(psState_type) + sizeof(meas_type));
                    crc = GetCrc(uart3->pTxBff, sizeof(psState_type) + sizeof(meas_type));
                    *(uint16_t*)(uart3->pTxBff + sizeof(psState_type) + sizeof(meas_type)) = crc;
                    uart_write(uart3, uart3->pTxBff, sizeof(psState_type) + sizeof(meas_type) + sizeof(uint16_t));
                    xSemaphoreTake(uartTxSem, pdMS_TO_TICKS(50));
                }else{
                    uartTsk.errorRequest++;
                }

                numRx = 0;
                uart_read(uart3, uart3->pRxBff, PieceBufRx);
            }
        }
    }
}

/************ (C) COPYRIGHT ************ END OF FILE ************ DEL ********/

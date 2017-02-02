/*!****************************************************************************
* @file    uart.h 
* @author  Storozhenko Roman - D_EL
* @version V1.3
* @date    09.01.2016
* @history 27-02-2015
* @history 26.03.2016 - remade for new gpio driver
* @brief   driver for uart of STM32L4 MCUs
*/
#ifndef UART_H
#define UART_H

#include "stm32l476xx.h"
#include "gpio.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "os_init.h"

/*!****************************************************************************
* User define
*/
//UART1
#define     UART1_USE                   (1)
#define     UART1_Tx_HOOK               (1)
#define     UART1_Rx_HOOK               (1)
#define     UART1_TxBffSz               (64)
#define     UART1_RxBffSz               (256)
#define     UART1_RxDmaInterruptPrior   (15)
#define     UART1_TXIRQPrior            (15)
#define     UART1_PINAFTX               (7)
#define     UART1_PINAFRX               (7)
#define     UART1_RX_IDLE_LINE_MODE     (0)
#define     UART1_RX_TIMEOUT_MODE       (1)
//UART2
#define     UART2_USE                   (1)
#define     UART2_Tx_HOOK               (1)
#define     UART2_Rx_HOOK               (1)
#define     UART2_TxBffSz               (64)
#define     UART2_RxBffSz               (256)
#define     UART2_RxDmaInterruptPrior   (15)
#define     UART2_TXIRQPrior            (15)
#define     UART2_PINAFTX               (7)
#define     UART2_PINAFRX               (7)
#define     UART2_RX_IDLE_LINE_MODE     (0)
#define     UART2_RX_TIMEOUT_MODE       (1)
//UART3
#define     UART3_USE                   (0)
#define     UART3_Tx_HOOK               (0)
#define     UART3_Rx_HOOK               (0)
#define     UART3_TxBffSz               (32)
#define     UART3_RxBffSz               (32)
#define     UART3_RxDmaInterruptPrior   (15)
#define     UART3_TXIRQPrior            (15)
#define     UART3_PINAFTX               (7)
#define     UART3_PINAFRX               (7)
#define     UART3_RX_IDLE_LINE_MODE     (0)
#define     UART3_RX_TIMEOUT_MODE       (0)

/*!****************************************************************************
* User enum
*/

/*!****************************************************************************
* User typedef
*/
typedef enum{
    BR9600,
    BR38400,
    BR115200
}usartBaudRate_type;

typedef enum{  
    uartTxFree,
    uartTxRun,
    uartTxSuccess,
    uartTxErr
}uartTxState_type;

typedef enum{  
    uartRxFree,
    uartRxRun,
    uartRxSuccess,
    uartRxStop,
    uartRxErr
}uartRxState_type;

typedef struct{
    USART_TypeDef           *pUart;
    uint8_t                 *pTxBff;
    uint8_t                 *pRxBff;
    DMA_Channel_TypeDef     *pUartTxDmaCh;
    DMA_Channel_TypeDef     *pUartRxDmaCh;
    uartTxState_type        txState     :8;
    uartRxState_type        rxState     :8;
    uint16_t                txCnt;
    uint16_t                rxCnt;
    uint16_t                errorRxCnt;
}uart_type;

/*!****************************************************************************
* Extern viriables
*/
extern uint32_t usartBaudRate[3];
#if (UART1_USE == 1)
extern uart_type            *uart1;
extern uint8_t         uart1TxBff[UART1_TxBffSz];
extern uint8_t         uart1RxBff[UART1_RxBffSz];
#endif //UART1_USE

#if (UART2_USE == 1)
extern uart_type            *uart2;
#endif //UART2_USE

#if (UART3_USE == 1)
extern uart_type            *uart3;
#endif //UART3_USE

/*!****************************************************************************
* Macro functions
*/
#define uartGetRemainTx(uartx)      (uartx->pUartTxDmaCh->CNDTR)
#define uartGetRemainRx(uartx)      (uartx->pUartRxDmaCh->CNDTR)

#define uart1RxHook(){                                                      \
    BaseType_t  xHigherPriorityTaskWoken;                                   \
    xHigherPriorityTaskWoken = pdFALSE;                                     \
    xSemaphoreGiveFromISR(semUart1Rx, &xHigherPriorityTaskWoken);           \
    if(xHigherPriorityTaskWoken != pdFALSE){                                \
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);                    \
    }                                                                       \
} 

#define uart1TxHook(){                                                      \
    BaseType_t  xHigherPriorityTaskWoken;                                   \
    xHigherPriorityTaskWoken = pdFALSE;                                     \
    xSemaphoreGiveFromISR(semUart1Tx, &xHigherPriorityTaskWoken);           \
    if(xHigherPriorityTaskWoken != pdFALSE){                                \
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);                    \
    }                                                                       \
} 

#define uart2RxHook(){                                                      \
    BaseType_t  xHigherPriorityTaskWoken;                                   \
    xHigherPriorityTaskWoken = pdFALSE;                                     \
    xSemaphoreGiveFromISR(semUart2Rx, &xHigherPriorityTaskWoken);           \
    if(xHigherPriorityTaskWoken != pdFALSE){                                \
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);                    \
    }                                                                       \
} 

#define uart2TxHook(){                                                      \
    BaseType_t  xHigherPriorityTaskWoken;                                   \
    xHigherPriorityTaskWoken = pdFALSE;                                     \
    xSemaphoreGiveFromISR(semUart2Tx, &xHigherPriorityTaskWoken);           \
    if(xHigherPriorityTaskWoken != pdFALSE){                                \
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);                    \
    }                                                                       \
} 

/*!****************************************************************************
* Prototypes for the functions
*/
void uart_init(USART_TypeDef *USARTx, usartBaudRate_type baudRate);
void uart_deinit(uart_type *uartx);
void uart_write(uart_type *uartx, void *src, uint16_t len);
void uart_read(uart_type *uartx, void *dst, uint16_t len);
void uart_stopRead(uart_type *uartStruct);

#endif //UART_H
/**************** (C) COPYRIGHT ************* END OF FILE ********* D_EL *****/

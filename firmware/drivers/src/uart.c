/*!****************************************************************************
* @file    uart.c 
* @author  Storozhenko Roman - D_EL
* @version V1.3
* @date    09.01.2016
* @history 27-02-2015
* @history 26.03.2016 - remade for new gpio driver
*
* UART1, use Tx DMA1_Channel4, use Rx DMA1_Channel5
* UART2, use Tx DMA1_Channel7, use Rx DMA1_Channel6
* UART3, use Tx DMA1_Channel2, use Rx DMA1_Channel3
*/
#include "uart.h"

/*!****************************************************************************
* uart1 memory
*/
#if (UART1_USE > 0)
uart_type       uart1Sct;
uart_type       *uart1 = &uart1Sct;
uint8_t         uart1TxBff[UART1_TxBffSz];
uint8_t         uart1RxBff[UART1_RxBffSz];
#endif //UART1_USE

/*!****************************************************************************
* uart2 memory
*/
#if (UART2_USE > 0)
uart_type       uart2Sct;
uart_type       *uart2 = &uart2Sct;
uint8_t         uart2TxBff[UART2_TxBffSz];
uint8_t         uart2RxBff[UART2_RxBffSz];
#endif //UART2_USE

/*!****************************************************************************
* uart3 memory
*/
#if (UART3_USE > 0)
uart_type       uart3Sct;
uart_type       *uart3 = &uart3Sct;
volatile uint8_t         uart3TxBff[UART3_TxBffSz];
volatile uint8_t         uart3RxBff[UART3_RxBffSz];
#endif //UART3_USE

uint16_t usartBaudRateDiv[3] = {
    0x09c4,                 //9600
    0x0271,                 //38400
    0x00D0,                 //115200
    //Добавить другие частоты
};

uint32_t usartBaudRate[3] = {
    9600,
    38400,
    115200,
    //Добавить другие частоты
};

/*!****************************************************************************
* @brief
*/
void uart_init(USART_TypeDef *USARTx, usartBaudRate_type baudRate){
    uart_type               *uartStruct;

    #if (UART1_USE > 0)
    if(USARTx == USART1){
        /************************************************
        * Memory setting
        */
        uartStruct = uart1;
        uartStruct->pUart        = USART1;
        uartStruct->pTxBff       = uart1TxBff;
        uartStruct->pRxBff       = uart1RxBff;
        uartStruct->pUartTxDmaCh = DMA1_Channel4;
        uartStruct->pUartRxDmaCh = DMA1_Channel5;
        
        /************************************************
        * IO
        */
        //PA9 USART1_TX
        gppin_init(GPIOA, 9, alternateFunctionPushPull, 0, UART1_PINAFTX);
        //PA10 USART1_RX
        gppin_init(GPIOA, 10, alternateFunctionPushPull, 0, UART1_PINAFRX);
        
        /************************************************
        * NVIC
        */
        NVIC_EnableIRQ(USART1_IRQn);
        NVIC_SetPriority(USART1_IRQn, UART1_TXIRQPrior);
        #if (UART1_RX_IDLE_LINE_MODE | UART1_RX_TIMEOUT_MODE == 0)
            NVIC_EnableIRQ(DMA1_Channel5_IRQn);                                 //Включить прерывания от DMA1_Channel 5
            NVIC_SetPriority(DMA1_Channel5_IRQn, UART1_RxDmaInterruptPrior);    //Установить приоритет
        #endif
        
        /************************************************
        * USART clock
        */
        RCC->APB2ENR    |= RCC_APB2ENR_USART1EN;                            //USART1 clock enable
        RCC->APB2RSTR   |= RCC_APB2RSTR_USART1RST;                          //USART1 reset
        RCC->APB2RSTR   &= ~RCC_APB2RSTR_USART1RST;
        
        /************************************************
        * DMA clock
        */
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
        
        /************************************************
        * DMA request settings
        */        
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C4S_Msk;        //Channel 4 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C4S_Pos;  //Channel 4 mapped on USART1_TX
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C5S_Msk;        //Channel 5 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C5S_Pos;  //Channel 5 mapped on USART1_RX
        
        /************************************************
        * USART IDLE interrupt enable
        */
        #if (UART1_RX_IDLE_LINE_MODE > 0)
            uartStruct->pUart->ICR  = USART_ICR_IDLECF;
            uartStruct->pUart->CR1  |= USART_CR1_IDLEIE;
        #endif
        
        /************************************************
        * USART TIMEOUT interrupt enable
        */
        #if (UART1_RX_TIMEOUT_MODE  > 0)
            uartStruct->pUart->ICR  = USART_ICR_RTOCF;
            uartStruct->pUart->RTOR = 1000;
            uartStruct->pUart->CR2  |= USART_CR2_RTOEN;
            uartStruct->pUart->CR1  |= USART_CR1_RTOIE;
        #endif
        
    }
    #endif //UART1_USE
    
    #if (UART2_USE > 0)
    if(USARTx == USART2){
        /************************************************
        * Memory setting
        */
        uartStruct = uart2;
        uartStruct->pUart        = USART2;
        uartStruct->pTxBff       = uart2TxBff;
        uartStruct->pRxBff       = uart2RxBff;
        uartStruct->pUartTxDmaCh = DMA1_Channel7;
        uartStruct->pUartRxDmaCh = DMA1_Channel6;
        
        /************************************************
        * IO
        */
        //PA2 USART2_TX
        gppin_init(GPIOA, 2, alternateFunctionPushPull, 0, UART2_PINAFTX);
        //PA3 USART2_RX
        gppin_init(GPIOA, 3, alternateFunctionPushPull, 0, UART2_PINAFRX);
        
        /************************************************
        * NVIC
        */
        NVIC_EnableIRQ(USART2_IRQn);
        NVIC_SetPriority(USART2_IRQn, UART2_TXIRQPrior);
        #if (UART2_RX_IDLE_LINE_MODE | UART2_RX_TIMEOUT_MODE == 0)
            NVIC_EnableIRQ(DMA1_Channel6_IRQn);                                 //Включить прерывания от DMA1_Channel 6
            NVIC_SetPriority(DMA1_Channel6_IRQn, UART2_RxDmaInterruptPrior);    //Установить приоритет
        #endif
        
        /************************************************
        * USART clock
        */
        RCC->APB1ENR1    |= RCC_APB1ENR1_USART2EN;                            //USART1 clock enable
        RCC->APB1RSTR1   |= RCC_APB1RSTR1_USART2RST;                          //USART1 reset
        RCC->APB1RSTR1   &= ~RCC_APB1RSTR1_USART2RST;
        
        /************************************************
        * USART IDLE interrupt enable
        */
        #if (UART2_RX_IDLE_LINE_MODE > 0)
            uartStruct->pUart->ICR    = USART_ICR_IDLECF;
            uartStruct->pUart->CR1   |= USART_CR1_IDLEIE;
        #endif
            
        /************************************************
        * USART TIMEOUT interrupt enable
        */
        #if (UART2_RX_TIMEOUT_MODE  > 0)
            uartStruct->pUart->ICR  = USART_ICR_RTOCF;
            uartStruct->pUart->RTOR = 1000;
            uartStruct->pUart->CR2  |= USART_CR2_RTOEN;
            uartStruct->pUart->CR1  |= USART_CR1_RTOIE;
        #endif
        
        /************************************************
        * DMA clock
        */
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
        /************************************************
        * DMA request settings
        */        
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C7S_Msk;        //Channel 7 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C7S_Pos;  //Channel 7 mapped on USART1_TX
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C6S_Msk;        //Channel 6 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C6S_Pos;  //Channel 6 mapped on USART1_RX
    }
    #endif //UART2_USE
    
    #if (UART3_USE > 0)
    if(USARTx == USART3){
        /************************************************
        * Memory setting
        */
        uartStruct = uart3;
        uartStruct->pUart        = USART3;
        uartStruct->pTxBff       = (uint8_t*)uart3TxBff;
        uartStruct->pRxBff       = (uint8_t*)uart3RxBff;
        uartStruct->pUartTxDmaCh = DMA1_Channel2;
        uartStruct->pUartRxDmaCh = DMA1_Channel3;
        
        /************************************************
        * IO
        */
        gppin_init(GPIOB, 10, alternateFunctionPushPull, 0, UART3_PINAFTX);  //PB10 USART3_TX
        gppin_init(GPIOB, 11, alternateFunctionPushPull, 0, UART3_PINAFRX);  //PB11 USART3_RX
        
        /************************************************
        * NVIC
        */
        NVIC_EnableIRQ(USART3_IRQn);
        NVIC_SetPriority(USART3_IRQn, UART3_TXIRQPrior);
        #if (UART3_RX_IDLE_LINE_MODE | UART3_RX_TIMEOUT_MODE == 0)
            NVIC_EnableIRQ(DMA1_Channel3_IRQn);                                 //Включить прерывания от DMA1_Channel 3
            NVIC_SetPriority(DMA1_Channel3_IRQn, UART3_RxDmaInterruptPrior);    //Установить приоритет
        #endif
        
        /************************************************
        * USART clock
        */
        RCC->APB1ENR1    |= RCC_APB1ENR1_USART3EN;                            //USART3 clock enable
        RCC->APB1RSTR1   |= RCC_APB1RSTR1_USART3RST;                          //USART3 reset
        RCC->APB1RSTR1   &= ~RCC_APB1RSTR1_USART3RST;
        
        /************************************************
        * USART IDLE interrupt enable
        */
        #if (UART3_RX_IDLE_LINE_MODE > 0)
            uartStruct->pUart->ICR    = USART_ICR_IDLECF;
            uartStruct->pUart->CR1   |= USART_CR1_IDLEIE;
        #endif
            
        /************************************************
        * USART TIMEOUT interrupt enable
        */
        #if (UART3_RX_TIMEOUT_MODE  > 0)
            uartStruct->pUart->ICR  = USART_ICR_RTOCF;
            uartStruct->pUart->RTOR = 1000;
            uartStruct->pUart->CR2  |= USART_CR2_RTOEN;
            uartStruct->pUart->CR1  |= USART_CR1_RTOIE;
        #endif
        
        /************************************************
        * DMA clock
        */
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
        
        /************************************************
        * DMA request settings
        */        
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C2S_Msk;        //Channel 2 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C2S_Pos;  //Channel 2 mapped on USART3_TX
        DMA1_CSELR->CSELR &= ~DMA_CSELR_C3S_Msk;        //Channel 3 clear
        DMA1_CSELR->CSELR |= 0x2 << DMA_CSELR_C3S_Pos;  //Channel 3 mapped on USART3_RX
    }
    #endif //UART3_USE
        
    /************************************************
    * USART
    */
    uartStruct->pUart->CR1      |= USART_CR1_UE;                                //Включаем USART
    uartStruct->pUart->CR1      &= ~USART_CR1_M;                                //Размерность слова данных - 8 бит
    uartStruct->pUart->CR2      &= ~USART_CR2_STOP;                             //1 стоп-бит

    uartStruct->pUart->BRR      = usartBaudRateDiv[baudRate];                   //Baud rate
    uartStruct->pUart->CR3      |= USART_CR3_DMAT;                              //DMA enable transmitter
    uartStruct->pUart->CR3      |= USART_CR3_DMAR;                              //DMA enable receiver

    uartStruct->pUart->CR1      |= USART_CR1_TE;                                //Transmitter enable
    uartStruct->pUart->CR1      |= USART_CR1_RE;                                //Receiver enable
    uartStruct->pUart->ICR       = USART_ICR_TCCF;                              //Clear the flag
    uartStruct->pUart->CR1      |= USART_CR1_TCIE;                              //Enable the interrupt
    
    /************************************************
    * DMA
    */
    //DMA Channel USART TX
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_EN;                               //Channel disabled
    uartStruct->pUartTxDmaCh->CCR |= DMA_CCR_PL_0;                              //Channel priority level - Medium
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_MSIZE;                            //Memory size - 8 bit
    uartStruct->pUartTxDmaCh->CCR |= DMA_CCR_PSIZE_1;                           //Peripheral size - 32 bit
    uartStruct->pUartTxDmaCh->CCR |= DMA_CCR_MINC;                              //Memory increment mode enabled
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_PINC;                             //Peripheral increment mode disabled
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_CIRC;                             //Circular mode disabled
    uartStruct->pUartTxDmaCh->CCR |= DMA_CCR_DIR;                               //Read from memory
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_TCIE;                              //Transfer complete interrupt disable
    uartStruct->pUartTxDmaCh->CNDTR = 0;                                        //Number of data
    uartStruct->pUartTxDmaCh->CPAR = (uint32_t)&(uartStruct->pUart->TDR);       //Peripheral address
    uartStruct->pUartTxDmaCh->CMAR = (uint32_t)NULL;                            //Memory address
    
    //DMA Channel USART RX
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_EN;                               //Channel disabled
    uartStruct->pUartRxDmaCh->CCR |= DMA_CCR_PL_0;                              //Channel priority level - Medium
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_MSIZE;                            //Memory size - 8 bit
    uartStruct->pUartRxDmaCh->CCR |= DMA_CCR_PSIZE_1;                           //Peripheral size - 32 bit
    uartStruct->pUartRxDmaCh->CCR |= DMA_CCR_MINC;                              //Memory increment mode enabled
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_PINC;                             //Peripheral increment mode disabled
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_CIRC;                             //Circular mode disabled
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_DIR;                              //Read from peripheral
    uartStruct->pUartRxDmaCh->CCR |= DMA_CCR_TCIE;                              //Transfer complete interrupt enable
    uartStruct->pUartRxDmaCh->CNDTR = 0;                                        //Number of data
    uartStruct->pUartRxDmaCh->CPAR = (uint32_t)&(uartStruct->pUart->RDR);       //Peripheral address
    uartStruct->pUartRxDmaCh->CMAR = (uint32_t)NULL;                            //Memory address
}

/*!****************************************************************************
* @brief
*/
void uart_deinit(uart_type *uartx){
    uartx->pUartTxDmaCh->CCR &= ~DMA_CCR_EN;               //Channel disabled
    uartx->pUartRxDmaCh->CCR &= ~DMA_CCR_EN;               //Channel disabled
    #if (UART1_USE > 0)
    if(uartx->pUart == USART1){
        RCC->APB2ENR    &= ~RCC_APB2ENR_USART1EN;           //USART1 clock disable
        NVIC_DisableIRQ(DMA1_Channel4_IRQn);
        NVIC_DisableIRQ(DMA1_Channel5_IRQn);   
    }
    #endif //UART1_USE
    
    #if (UART2_USE > 0)
    if(uartx->pUart == USART2){
        RCC->APB1ENR1    &= ~RCC_APB1ENR1_USART2EN;           //USART1 clock disable
        NVIC_DisableIRQ(DMA1_Channel6_IRQn);
        NVIC_DisableIRQ(DMA1_Channel7_IRQn);  
    }
    #endif //UART2_USE
    
    #if (UART3_USE > 0)
    if(uartx->pUart == USART3){
        RCC->APB1ENR1    &= ~RCC_APB1ENR1_USART3EN;       //USART1 clock disable
        NVIC_DisableIRQ(DMA1_Channel2_IRQn);
        NVIC_DisableIRQ(DMA1_Channel3_IRQn);   
    }
    #endif //UART3_USE
}

/*!****************************************************************************
* @brief
*/
void uart_write(uart_type *uartStruct, void *src, uint16_t len){
    uartStruct->pUartTxDmaCh->CCR &= ~DMA_CCR_EN;                               //Channel disabled
    uartStruct->pUartTxDmaCh->CMAR = (uint32_t)src;                             //Memory address
    uartStruct->pUartTxDmaCh->CNDTR = len;                                      //Number of data
    uartStruct->pUartTxDmaCh->CCR |= DMA_CCR_EN;                                //Channel enabled
    uartStruct->txState = uartTxRun;
}

/******************************************************************************
*
*/
void uart_read(uart_type *uartStruct, void *dst, uint16_t len){
    //uartStruct->pUart->ICR = USART_ICR_RTOCF;
    uartStruct->pUart->ICR = 0xFFFFFFFF;
    (void)uartStruct->pUart->RDR;
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_EN;                               //Channel disabled
    uartStruct->pUartRxDmaCh->CMAR = (uint32_t)dst;                             //Memory address
    uartStruct->pUartRxDmaCh->CNDTR = len;                                      //Number of data
    uartStruct->pUartRxDmaCh->CCR |= DMA_CCR_EN;                                //Channel enabled
    uartStruct->rxState = uartRxRun;
}

/******************************************************************************
*
*/
void uart_stopRead(uart_type *uartStruct){
    uartStruct->pUartRxDmaCh->CCR &= ~DMA_CCR_EN;                               //Channel disabled
    uartStruct->rxState = uartRxStop;
}

/******************************************************************************
* Transfer complete interrupt USART1_IRQn (USART1 TX and IDLE RX)
*/
#if (UART1_USE > 0)
__irq void USART1_IRQHandler(void){
    uint16_t uartsr = uart1->pUart->ISR;
    
    if((uartsr & USART_ISR_TC) != 0){
        DMA1_Channel4->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart1->txState  = uartTxSuccess;
        uart1->txCnt++;
        #if (UART1_Tx_HOOK > 0)
        uart1TxHook();
        #endif //UART1_Tx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF4;                                      //Clear flag
        uart1->pUart->ICR |= USART_ICR_TCCF;
    }
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART1_RX_IDLE_LINE_MODE  > 0)
    if((uartsr & USART_ISR_IDLE) != 0){
        DMA1_Channel5->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart1->rxState  = uartRxSuccess;
        uart1->rxCnt++;
        #if (UART1_Rx_HOOK > 0)
        uart1RxHook();
        #endif //UART1_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF5;                                      //Clear flag
        uart1->pUart->ICR = USART_ICR_IDLECF;
    }
    #endif
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART1_RX_TIMEOUT_MODE  > 0)
    if((uartsr & USART_ISR_RTOF) != 0){
        DMA1_Channel5->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart1->rxState  = uartRxSuccess;
        uart1->rxCnt++;
        #if (UART1_Rx_HOOK > 0)
        uart1RxHook();
        #endif //UART1_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF5;                                      //Clear flag
        uart1->pUart->ICR = USART_ICR_RTOCF;
    }
    #endif
}
/******************************************************************************
* Transfer complete interrupt DMA1_Channel5 (USART1 RX)
*/
__irq void DMA1_Channel5_IRQHandler(void){
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
    uart1->rxState  = uartRxSuccess;
    uart1->rxCnt++;
    #if (UART1_Rx_HOOK > 0)
    uart1RxHook();
    #endif //UART1_Rx_HOOK
    DMA1->IFCR      = DMA_IFCR_CTCIF5;                                      //Clear flag
}
#endif //UART1_USE

/******************************************************************************
* Transfer complete interrupt USART2_IRQn (USART2 TX and IDLE RX)
*/
#if (UART2_USE > 0)
__irq void USART2_IRQHandler(void){
    uint16_t uartsr = uart2->pUart->ISR;
    
    if((uartsr & USART_ISR_TC) != 0){
        DMA1_Channel7->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart2->txState  = uartTxSuccess;
        uart2->txCnt++;
        #if (UART2_Tx_HOOK > 0)
        uart2TxHook();
        #endif //UART2_Tx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF7;                                      //Clear flag
        uart2->pUart->ICR |= USART_ICR_TCCF;
    }
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART2_RX_IDLE_LINE_MODE  > 0)
    if((uartsr & USART_ISR_IDLE) != 0){
        DMA1_Channel6->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart2->rxState  = uartRxSuccess;
        uart2->rxCnt++;
        #if (UART2_Rx_HOOK > 0)
        uart2RxHook();
        #endif //UART2_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF6;                                      //Clear flag
        uart2->pUart->ICR = USART_ICR_IDLECF;
    }
    #endif
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART2_RX_TIMEOUT_MODE  > 0)
    if((uartsr & USART_ISR_RTOF) != 0){
        DMA1_Channel6->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart2->rxState  = uartRxSuccess;
        uart2->rxCnt++;
        #if (UART2_Rx_HOOK > 0)
        uart2RxHook();
        #endif //UART2_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF6;                                      //Clear flag
        uart2->pUart->ICR = USART_ICR_RTOCF;
    }
    #endif
}
/******************************************************************************
* Transfer complete interrupt DMA1_Channel6 (USART2 RX)
*/
__irq void DMA1_Channel6_IRQHandler(void){
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
    uart2->rxState  = uartRxSuccess;
    uart2->rxCnt++;
    #if (UART2_Rx_HOOK > 0)
    uart2RxHook();
    #endif //UART2_Rx_HOOK
    DMA1->IFCR      = DMA_IFCR_CTCIF6;                                      //Clear flag
}
#endif //UART2_USE

/******************************************************************************
* Transfer complete interrupt USART3_IRQn (USART3 TX and IDLE RX)
*/
#if (UART3_USE > 0)
__irq void USART3_IRQHandler(void){
    uint16_t uartsr = uart3->pUart->ISR;
    
    if((uartsr & USART_ISR_TC) != 0){
        DMA1_Channel2->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart3->txState  = uartTxSuccess;
        uart3->txCnt++;
        #if (UART3_Tx_HOOK > 0)
        uart3TxHook();
        #endif //UART3_Tx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF2;                                      //Clear flag
        uart3->pUart->ICR |= USART_ICR_TCCF;
    }
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART3_RX_IDLE_LINE_MODE  > 0)
    if((uartsr & USART_ISR_IDLE) != 0){
        DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart3->rxState  = uartRxSuccess;
        uart3->rxCnt++;
        #if (UART3_Rx_HOOK > 0)
        uart3RxHook();
        #endif //UART3_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF3;                                      //Clear flag
        uart3->pUart->ICR = USART_ICR_IDLECF;
    }
    #endif
    /************************************************
    * USART TIMEOUT interrupt enable
    */
    #if (UART3_RX_TIMEOUT_MODE  > 0)
    if((uartsr & USART_ISR_RTOF) != 0){
        DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
        uart3->rxState  = uartRxSuccess;
        uart3->rxCnt++;
        #if (UART3_Rx_HOOK > 0)
        uart3RxHook();
        #endif //UART3_Rx_HOOK
        DMA1->IFCR      = DMA_IFCR_CTCIF3;                                      //Clear flag
        uart3->pUart->ICR = USART_ICR_RTOCF;
    }
    #endif
}
/******************************************************************************
* Transfer complete interrupt DMA1_Channel3 (USART3 RX)
*/
__irq void DMA1_Channel3_IRQHandler(void){
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;                                     //Channel disabled
    uart3->rxState  = uartRxSuccess;
    uart3->rxCnt++;
    #if (UART3_Rx_HOOK > 0)
    uart3RxHook();
    #endif //UART3_Rx_HOOK
    DMA1->IFCR      = DMA_IFCR_CTCIF3;                                      //Clear flag
}
#endif //UART3_USE

/******************* (C) COPYRIGHT ***************** END OF FILE ********* D_EL *****/

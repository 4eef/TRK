/*!****************************************************************************
* @file    gsmTSK.c
* @author  4eef
* @version V1.0
* @date    19-Aug-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "gsmTSK.h"

/*!****************************************************************************
* MEMORY
*/
extern gpsData_type gpsData;
static char       *strings[] = {
    "\r\nOK\r\n",
    "\r\n+CMTI:",
    "GPS?",
    "GPS is OFF",
    "GPS is ON but not ready",
    "POS: 00.00\'00.00\"U, 00.00\'00.00\"U;\r\nSPD: 000.00 Kph;\r\nALT: 0000 U;\r\nHDOP: 00.00;\r\nDATE: 0000-00-00 00:00:00 GMT;",
    "GPSON",
    "GPSOFF",
    "OK",
    "AT+CMGR=0000\r",
    "AT+CMGDA=1\r",
    "AT+CMGDA=3\r",
    "AT+CMGF=0\r",
    "AT+CMGS=0000\r",
    "\r\n>",
    "\r\n+CMGS:",
    "\r\nRING\r\n",
    "\r\n+CMS ERROR:",
    "\r\nERROR\r\n",
    "ATH\r",
    "AT\r",
    "AT+GMM\r",
    "AT+GMR\r",  
    "AT+GSN\r",
    "AT+CMGL=0\r",
    "AT+CMGL=2\r",
    "AT+CMGW=0000\r",
    "AT+CMSS=0000\r",
    "\r\n+CMGW:",
    "\r\n+CMSS:",
    "AT+CMGDA=6\r",
    "AT+CMGD=0000\r",
    "AT+CMGL=1\r",
};

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
char * string(strings_enum_type txt){
    return strings[txt];
}

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void gsm_sendCommand(strings_enum_type cmd){
    extern char *trxbuffGsm;
    int     len = 0;
    len = strlen(string(cmd));                               //Calculate the length
    memset(&trxbuffGsm[0], 0, sizeof(trxbuffGsm));        
    memcpy(trxbuffGsm, string(cmd), len);                    //Copy the command to UART buffer
    uart_write(uart1, trxbuffGsm, len);                       //Perform the transfer
    xSemaphoreTake(semUart1Tx, pdMS_TO_TICKS(500));             //Wait until transfer completes
}

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
extern tm_type tm;
gsmData_type gsmData;
static char *trxbuffGsm;//, *pTmpGsm;
uint16_t rxBffSzGsm = UART2_RxBffSz;
char rxtmpGsm[UART1_RxBffSz], rcvd[50];

void gsmTSK(void *pPrm){
    BaseType_t  res;
    trxbuffGsm = &rxtmpGsm[0];
    gsmData.gsmStatus.gsmError = 0;
    gsmData.gsmStatus.gsmOn = 0;
    gsmData.gsmStatus.gsmSet = 0;
    gsmData.gsmStatus.gsmOk = 0;
    
    gsmData.gsmStatus.gsmOn = 1;                                        //For debug purposes
    gsmData.txSMS.sentSmsCnt = 0;
    gsmData.rxSMS.rcvdSmsCnt = 0;
    gsmData.rxSMS.smsUnread = 0;
    gsmData.txSMS.smsUnsent = 0;
    gsmData.txSMS.txFailed = 0;
    
    while(1){
        uart_read(uart1, uart1->pRxBff, rxBffSzGsm);
        gsm_sendCommand(AT);
        res = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
        if(res == pdTRUE){
            char *tmp = string(ansOk);
            uint8_t i, itmp = 0, shift;
            shift = strlen(string(AT));
            for(i = 0; i < strlen(tmp); i++) {
                if (tmp[i] == uart1->pRxBff[i+shift]){
                    itmp++;
                }
            }
            if(itmp == strlen(tmp)){
                //Get model ID
                uart_read(uart1, uart1->pRxBff, rxBffSzGsm);
                gsm_sendCommand(getModelId);
                BaseType_t res1 = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
                if(res1 == pdTRUE){
                    memcpy(trxbuffGsm, uart1->pRxBff, rxBffSzGsm);      //Compare first nine chars
                    trxbuffGsm += 9;
                    uint8_t len = strlen(trxbuffGsm);
                    memcpy(gsmData.modelInfo, trxbuffGsm, len-8);       //Compare first nine chars
                }
                //Get firmware rivision
                uart_read(uart1, uart1->pRxBff, rxBffSzGsm);
                gsm_sendCommand(getFirmRev);
                BaseType_t res2 = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
                if(res2 == pdTRUE){
                    memcpy(trxbuffGsm, uart1->pRxBff, rxBffSzGsm);      //Compare first nine chars
                    trxbuffGsm += 9;
                    uint8_t len = strlen(trxbuffGsm);
                    memcpy(gsmData.firmRev, trxbuffGsm, len-8);         //Compare first nine chars
                }
                //Get IMEI
                uart_read(uart1, uart1->pRxBff, rxBffSzGsm);
                gsm_sendCommand(getIMEI);
                BaseType_t res3 = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
                if(res3 == pdTRUE){
                    memcpy(trxbuffGsm, uart1->pRxBff, rxBffSzGsm);      //Compare first nine chars
                    trxbuffGsm += 9;
                    uint8_t len = 15;                                   //IMEI length
                    memcpy(gsmData.IMEI, trxbuffGsm, len);              //Compare first nine chars
                }
                if(res1 && res2 && res3 != 0){                          //If all info received - go to main cycle
                    gsmData.gsmStatus.gsmOk = 1;
                    break;
                }
            }
        }
    }
    while(1){
        uint8_t i, itmp, state;
        uint16_t tout;
        char *tmp, tmpbuff[sizeof(gsmData.rxSMS.charPDU)];
        uart_read(uart1, uart1->pRxBff, rxBffSzGsm);                    //Call function only if semaphore has read
        vTaskDelay(pdMS_TO_TICKS(1));
        while(1){
            res = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1));
            if(res == pdTRUE){                                              //Something is received
                //SMS is received
                tmp = string(smsRcvd);
                itmp = 0;
                for(i = 0; i < strlen(tmp); i++) {                          //Compare with temporary string
                    if (tmp[i] == uart1->pRxBff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(tmp)){
                    rcvSms();                                               //Parse module to receive SMS
                    gsmData.gsmStatus.gsmSmsRcvd = 1;                       //Set the "SMS received flag"
                }
                //RING is received
                tmp = string(incoming);
                itmp = 0;
                for(i = 0; i < strlen(tmp); i++) {                          //Compare with temporary string
                    if (tmp[i] == uart1->pRxBff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(tmp)){
                    //Discard the call
                    tout = 1000;
                    gsmSendString(tmpbuff, string(discardCall), tout);
                    gsmData.gsmStatus.gsmCallMssd = 1;
                }
                break;
            }
            //Check if there is SMS to send
            if(gsmData.gsmStatus.gsmSmsTxRdy != 0){
                while(1){
                    state = sndSms();
                    if(state != 0){
                        if(gsmData.txSMS.txFailed >= 3){
                            gsmData.gsmStatus.gsmSmsTxRdy = 0;
                            break;                                          //If SMS re-TX is failed after three retransmits - break cycle
                        }
                    }else{
                        if(gsmData.txSMS.smsUnsent == 0){
                            gsmData.gsmStatus.gsmSmsTxRdy = 0;                  //SMS transmitted successfully
                            break;
                        }
                    }
                }
            }
            //Check if there is unread SMS
            if(gsmData.rxSMS.smsUnread != 0){
                rcvSms();                                               //Parse module to receive SMS
                gsmData.gsmStatus.gsmSmsRcvd = 1;                       //Set the "SMS received flag"
            }
        }
    }
}

/*!****************************************************************************
* @brief   GSM string TRX routine
* @param    
* @retval   
*/
uint8_t gsmSendString(char *dst, char *src, uint32_t tout){
    BaseType_t res;
    uint8_t state;
    //Clear the RX buffer
    memset(uart1->pRxBff, 0, sizeof(uart1RxBff));
    //Transfer
    uart_read(uart1, uart1->pRxBff, sizeof(uart1RxBff));                //Receive the echo
    uart_write(uart1, src, strlen(src));                                //Perform the transfer
    xSemaphoreTake(semUart1Tx, pdMS_TO_TICKS(1000));
    xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(1000));
    uart_read(uart1, uart1->pRxBff, sizeof(uart1RxBff));                //Receive the answer
    res = xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(tout));
    if(res == pdTRUE){
        state = 1;
    }else{
        state = 0;
    }
    //Copy the return string
    strcpy(dst, (char *)uart1->pRxBff);
    return state;
}

/*!****************************************************************************
* @brief   GPS string filling routine
* @param    
* @retval   
*/
char * fillGpsString(void){
    static char tmpStr[160];
    xttotm(gpsData.unixTime);
    sprintf(tmpStr, 
"POS: %02u %02u\'%02.2f\"%c %02u %02u\'%02.2f\"%c\r\n\
SPD: %.2f Kph\r\n\
ALT: %.0f %c\r\n\
HDOP: %.2f\r\n\
DATE: %04u/%02u/%02u %02u:%02u:%02u GMT",
                        gpsData.lat_degrees, gpsData.lat_minutes, gpsData.lat_seconds, gpsData.lat_sign, gpsData.lon_degrees, gpsData.lon_minutes, gpsData.lon_seconds, gpsData.lon_sign,
                        gpsData.speedKph,
                        gpsData.altitude, gpsData.altUnit,
                        gpsData.HDOP,
                        tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return tmpStr;
}

/*!****************************************************************************
* @brief   Received SMS parsing routine
* @param    
* @retval   
*/
uint8_t rcvSms(void){
    uint32_t tout;
    uint16_t j, len, pos;
    uint8_t state;
    int16_t i;
    gsmData.rxSMS.rdngSms = 1;
    gsmData.rxSMS.smsUnread = 0;
    char tmpbuff[sizeof(gsmData.rxSMS.charPDU)], *pTmp;
    //Get first unread SMS number
    tout = 1000;                                                        //Timeout in ms
    gsmSendString(tmpbuff, string(getUnrLst), tout);
    len = 50;                                                           //String max length
    for(i = 0; i < len; i++){                                           //Get the comma position
        if(tmpbuff[i] == ','){
            pos = i - 1;
            break;
        }
    }
    memset(gsmData.rxSMS.rxSmsNum, '0', sizeof(gsmData.rxSMS.rxSmsNum));
    j = sizeof(gsmData.rxSMS.rxSmsNum);                                 //Digit start position
    for(i = pos; i > 0; i--){
        if(tmpbuff[i] == ' ') break;
        gsmData.rxSMS.rxSmsNum[j - 1] = tmpbuff[i];
        j--;
    }
    //Read the SMS
    pTmp = string(readMsg);
    j = strlen(pTmp);                                                   //Last digit position
    len = 4;                                                            //Number of digits
    memset(tmpbuff, 0, sizeof(tmpbuff));
    //Copy the string
    strcpy(tmpbuff, pTmp);
    //Ñopy the number of received SMS
    for(i = (len-1); i >= 0; i--){                                      //Edit length
        tmpbuff[j-2] = gsmData.rxSMS.rxSmsNum[i];
        j--;
    }
    gsmSendString(tmpbuff, tmpbuff, tout); 
    //Parse the received string
    len = strlen(tmpbuff);
    memset(gsmData.rxSMS.charPDU, 0, sizeof(gsmData.rxSMS.charPDU));
    gsmData.rxSMS.pduLen = 0;
    j = 0;
    state = 0;
    for(i = 0; i < len; i++){
        if(tmpbuff[i] == ':') state = 1;
        if(state != 0){
            if(tmpbuff[i-1] == '\n') state = 2;
            if(state == 2){
                gsmData.rxSMS.charPDU[j] = tmpbuff[i];
                gsmData.rxSMS.pduLen++;
                j++;
                if(tmpbuff[i+1] == '\r') break;
            }
        }
    }
    j = gsmData.rxSMS.pduLen - 1;
    string2pdu();                                                       //Parse PDU string
    if(gsmData.rxSMS.msgLength != 0){                                   //Look the message length and return the corresponding value
        gsmData.rxSMS.rcvdSmsCnt++;
        state = 0;
    }else{
        state = 1;
    }
    //Delete read messages
    gsmSendString(tmpbuff, string(delReadMsgs), tout);
    //Check if there are more messages
    tout = 100;
    if(gsmData.rxSMS.smsUnread == 0){
        gsmSendString(tmpbuff, string(getReadLst), tout);               //If messages are read
    }else{
        gsmSendString(tmpbuff, string(getUnrLst), tout);                //If messages are unread
    }
    //If a lot of messages unread (up to 30)
    for(i = 0; i < 30; i++){
        xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(100));
    }
    //Find the ":" character present in unread message string
    len = strlen(tmpbuff);
    for(i = 0; i < len; i++){
        if(tmpbuff[i] == ':'){
            gsmData.rxSMS.smsUnread = 1;
            break;
        }else{
            gsmData.rxSMS.smsUnread = 0;
        }
    }
    gsmData.rxSMS.rdngSms = 0;
    return state;
}

/*!****************************************************************************
* @brief   SMS sending routine
* @param    
* @retval   
*/
uint8_t sndSms(void){//char *str, uint8_t *num){
    uint32_t tout;
    uint16_t j, state = 0, len, itmp, shift, pos;
    int16_t i;
    char tmpbuff[sizeof(gsmData.rxSMS.charPDU)], *pTmp;
    gsmData.txSMS.sndngSms = 1;                                         //Set the "Sending SMS flag"
    pdu2string();                                                       //Encode the PDU string
    //Switch to PDU mode
    tout = 1000;                                                        //Timeout in ms
    gsmSendString(tmpbuff, string(pduMode), tout);
    //Message length
    uint8_t ui8msglen[2];
    ui8msglen[0] = int2bcd(gsmData.txSMS.pduLen - 1)>>8 & 0xFF;
    ui8msglen[1] = int2bcd(gsmData.txSMS.pduLen - 1) & 0xFF;
    hex2char(gsmData.txSMS.txPduLen, ui8msglen, 2);
    if(gsmData.txSMS.smsUnsent == 0){
        //Prepare to send the PDU string
        pTmp = string(saveMsg);
        j = strlen(pTmp);                                                   //Last digit position
        len = 4;                                                            //Number of digits
        memset(tmpbuff, 0, sizeof(tmpbuff));
        //Copy the string
        strcpy(tmpbuff, pTmp);
        //Ñopy the number of received SMS
        for(i = (len-1); i >= 0; i--){                                      //Edit length
            tmpbuff[j-2] = gsmData.txSMS.txPduLen[i];
            j--;
        }
        gsmSendString(tmpbuff, tmpbuff, tout);                              //Perform transfer
        //Check if ">" is received
        pTmp = string(wellcome);
        //Compare received string with temporary
        itmp = 0;
        shift = strlen(string(sendMsg));
        for(i = 0; i < strlen(pTmp); i++) {
            if (pTmp[i] == tmpbuff[i+shift]){
                itmp++;
            }
        }
        if(itmp == strlen(pTmp)){
            //Transfer PDU string
tx:         gsmSendString(tmpbuff, (char *)gsmData.txSMS.charPDU, tout);    //Perform transfer
            //Check if message is stored
            pTmp = string(msgStored);
            //Compare received string with temporary
            itmp = 0;
            for(i = 0; i < strlen(pTmp); i++) {
                if (pTmp[i] == tmpbuff[i]){
                    itmp++;
                }
            }
            if(itmp != strlen(pTmp)){
                gsmData.txSMS.txFailed++;
                state = 1;
            }
        }else{
            while(1){
                //Check if SMS is received
                pTmp = string(smsRcvd);
                itmp = 0;
                for(i = 0; i < strlen(pTmp); i++){
                    if (pTmp[i] == uart1->pRxBff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(pTmp)){
                    gsmData.rxSMS.smsUnread = 1;
                    uart_read(uart1, uart1->pRxBff, rxBffSzGsm);        //Receiving the string
                    xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(60000));
                    goto tx;
                }
                //Check if "RING" is received
                pTmp = string(incoming);
                itmp = 0;
                for(i = 0; i < strlen(pTmp); i++){
                    if (pTmp[i] == uart1->pRxBff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(pTmp)){
                    //Discard the call
                    tout = 1000;
                    gsmSendString(tmpbuff, string(discardCall), tout);
                    gsmData.gsmStatus.gsmCallMssd = 1;
                    gsmData.txSMS.txFailed++;
                    state = 3;
                    break;
                }
                //Check if "CMS ERROR" is received
                pTmp = string(cmsErr);
                itmp = 0;
                for(i = 0; i < strlen(pTmp); i++){
                    if (pTmp[i] == uart1->pRxBff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(pTmp)){
                    //Discard the call
                    tout = 1000;
                    gsmSendString(tmpbuff, string(discardCall), tout);
                    gsmData.gsmStatus.gsmCallMssd = 1;
                    gsmData.txSMS.txFailed++;
                    state = 3;
                    break;
                }
                //If no match - delete unsent SMS (probable overflow)
                tout = 25000;
                gsmSendString(tmpbuff, string(delAllMsgs), tout);
                pTmp = string(ansOk);                                   //Check if SMS are deleted
                itmp = 0;
                for(i = 0; i < strlen(pTmp); i++){
                    if (pTmp[i] == tmpbuff[i]){
                        itmp++;
                    }
                }
                if(itmp == strlen(pTmp)){
                    gsmData.txSMS.txFailed++;
                    state = 4;
                    break;
                }
            }
        }
    }
    if(state == 0){
        //Get first unsent message number
        tout = 100;
        gsmSendString(tmpbuff, string(getUnsntLst), tout);
        //If a lot of messages unsent (up to 30)
        for(i = 0; i < 30; i++){
            xSemaphoreTake(semUart1Rx, pdMS_TO_TICKS(100));
        }
        //Copy the first unsent SMS number
        len = 50;                                                   //String max length
        for(i = 0; i < len; i++){                                   //Get the comma position
            if(tmpbuff[i] == ','){
                pos = i - 1;
                break;
            }
        }
        memset(gsmData.txSMS.txSmsNum, '0', sizeof(gsmData.txSMS.txSmsNum));
        j = sizeof(gsmData.txSMS.txSmsNum);                         //Digit start position
        for(i = pos; i > 0; i--){
            if(tmpbuff[i] == ' ') break;
            gsmData.txSMS.txSmsNum[j - 1] = tmpbuff[i];
            j--;
        }
        //Send the SMS
        pTmp = string(sendStMsg);
        j = strlen(pTmp);                                           //Last digit position
        len = 4;                                                    //Number of digits
        memset(tmpbuff, 0, sizeof(tmpbuff));
        strcpy(tmpbuff, pTmp);
        for(i = (len-1); i >= 0; i--){                              //Copy SMS number
            tmpbuff[j-2] = gsmData.txSMS.txSmsNum[i];
            j--;
        }
        tout = 60000;                                               //Timeout in ms for SMS transfer
        gsmSendString(tmpbuff, tmpbuff, tout);
        //Check if SMS is sent
        pTmp = string(stMsgSent);
        itmp = 0;
        for(i = 0; i < strlen(pTmp); i++){
            if (pTmp[i] == uart1->pRxBff[i]){
                itmp++;
            }
        }
        if(itmp == strlen(pTmp)){
            gsmData.txSMS.sentSmsCnt++;
            gsmData.txSMS.txFailed = 0;                             //Reset the retranslation counter
            state = 0;
        }
        //Check if "ERROR" is received
        pTmp = string(txErr);
        itmp = 0;
        for(i = 0; i < strlen(pTmp); i++){
            if (pTmp[i] == uart1->pRxBff[i]){
                itmp++;
            }
        }
        if(itmp == strlen(pTmp)){
            gsmData.txSMS.txFailed++;
            state = 2;
        }
    }
    //Clear sent SMS
    tout = 1000;
    gsmSendString(tmpbuff, string(delSentMsgs), tout);
    //Check if there are unsent messages
    tout = 1000;
    gsmSendString(tmpbuff, string(getUnsntLst), tout);
    //Find the ":" character present in unread message string
    len = strlen(tmpbuff);
    for(i = 0; i < len; i++){
        if(tmpbuff[i] == ':'){
            gsmData.txSMS.smsUnsent = 1;
            break;
        }else{
            gsmData.txSMS.smsUnsent = 0;
        }
    }
    gsmData.txSMS.sndngSms = 0;                                         //Clear the "Sending SMS flag"
    return state;
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

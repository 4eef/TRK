/*!****************************************************************************
* @file    pduenc.c
* @author  4eef
* @version V1.0
* @date    23-Aug-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "pduenc.h"

/*!****************************************************************************
* MEMORY
*/
extern gsmData_type gsmData;
extern tm_type tm;

/*!***************************************************************************
* @brief    Convert bcd to char string
* @param
*/
//void hex2char(uint8_t *dst, uint16_t *src, uint8_t len){
//    if(len == 0) return;
//    uint8_t nibble[4], i, j, k = 0;
//    for(j = 0; j < len; j++){                               //Convert hexs to chars
//        nibble[0] = src[j] >> 4;
//        nibble[1] = src[j] & 0xF;
//        for(i = 0; i < 2; i++){
//            if((nibble[i] <= 0 || nibble[i] <= 9) != 0){
//                dst[k] = '0' + nibble[i];
//            }else{
//                dst[k] = 'A' + (nibble[i] - 0xA);
//            }
//            k++;
//        }
//    }
//}

/*!***************************************************************************
* @brief    Convert hex string to char string
* @param
*/
void hex2char(uint8_t *dst, uint8_t *src, uint8_t len){
    if(len == 0) return;
    uint8_t nibble[2], i, j, k = 0;
    for(j = 0; j < len; j++){                               //Convert hexs to chars
        nibble[0] = src[j] >> 4;
        nibble[1] = src[j] & 0xF;
        for(i = 0; i < 2; i++){
            if((nibble[i] <= 0 || nibble[i] <= 9) != 0){
                dst[k] = '0' + nibble[i];
            }else{
                dst[k] = 'A' + (nibble[i] - 0xA);
            }
            k++;
        }
    }
}

/*!***************************************************************************
* @brief    Convert char string to hex string
* @param
*/
void char2hex(uint8_t *dst, uint8_t *src, uint8_t len){
    if(len == 0) return;
    uint8_t i = 0, nibble = 0, hex = 0;
    for(i = 0; i < len; i++){                               //Convert chars to hexs
        if(src[i] < 'A'){
            nibble = src[i] - '0';                          //Digits
        }else{
            nibble = src[i] - '7';                          //Letters
        }
        if((i % 2) == 0){
            hex = nibble<<4;
        }else{
            hex |= nibble;
            dst[i/2] = hex;
        }
    }
}

/*!***************************************************************************
* @brief    Convert PDU string to char string
* @param
*/
void pdu2string(void){
    pduEncode();
    hex2char(gsmData.txSMS.charPDU, gsmData.txSMS.PDU, gsmData.txSMS.pduLen);
    gsmData.txSMS.charPDU[gsmData.txSMS.pduLen*2] = 0x1A;   //Send command
}

/*!***************************************************************************
* @brief    Convert char string to PDU string
* @param
*/
void string2pdu(void){
    uint8_t *pStart, *pEnd, len;
    pStart = &gsmData.rxSMS.pduType;
    pEnd = &gsmData.rxSMS.charPDU[0];
    len = pEnd - pStart - 1;
    memset(&gsmData.rxSMS.pduType, 0, len);
    char2hex(gsmData.rxSMS.PDU, gsmData.rxSMS.charPDU, gsmData.rxSMS.pduLen);
    pduDecode();
}

/*!***************************************************************************
* @brief    Compose the receiver number to BCDs
* @param
*/
//uint8_t * rcvrNumCmp(uint8_t *chars){
//    static uint8_t num[6];
//    uint8_t i;
//    for(i = 0; i < gsmData.rxSMS.sndrNumLen; i++){
//        gsmData.txSMS.rcvrNum[i] = (sendernum[i/2] & 0xF0)>>4;
//        gsmData.txSMS.rcvrNum[i+1] = sendernum[i/2] & 0xF;
//        i++;
//    }
//    return num;
//}

/*!***************************************************************************
* @brief    Decompose the sender number to bytes
* @param
*/
void sndrNumCmp(uint8_t *sendernum){
    uint8_t i;
    for(i = 0; i < gsmData.rxSMS.sndrNumLen; i++){
        gsmData.rxSMS.sndrNum[i] = (sendernum[i/2] & 0xF0)>>4;
        gsmData.rxSMS.sndrNum[i+1] = sendernum[i/2] & 0xF;
        i++;
    }
}

/*!***************************************************************************
* @brief    Compose the receiver number to bytes
* @param
*/
//uint8_t * rcvrNumCmp(void){
//    static uint8_t num[6];
//    uint8_t i;
//    if((gsmData.txSMS.rcvrNumLen % 2) != 0) gsmData.txSMS.rcvrNum[gsmData.txSMS.rcvrNumLen] = 0xF;
//    for(i = 0; i < gsmData.txSMS.rcvrNumLen; i++){
//        num[i/2] = gsmData.txSMS.rcvrNum[i + 1]<<4 | gsmData.txSMS.rcvrNum[i];
//        i++;
//    }
//    return num;
//}

/*!***************************************************************************
* @brief    Encodes ASCII text to GSM standard alphabet. Returns -1 for success, 0 for failure.
* @param
*/
void gsmDefaultEncode(uint8_t *str, uint8_t *pdu, uint16_t len){
    uint8_t i, octet, bits = 0;;
    
    for(i = 0; i < len; i++){
        if(bits == 7){
            bits = 0;
            continue;
        }
        octet = (str[i] & 0x7f) >> bits;
        if(i < len - 1) octet |= str[i + 1] << (7 - bits);
        *pdu++ = octet;
        bits++;
    }
}

/*!***************************************************************************
* @brief    Encodes data to PDU format
* @param
*/
void pduEncode(void){
    uint8_t msgPdu[140], *pOut, *pMsgPdu, msgLen, i;//, *pRcvrNum, rcvrNum[6];
    float ftmp;
    pOut = &gsmData.txSMS.PDU[0];
    pMsgPdu = &msgPdu[0];
    //pRcvrNum = &rcvrNum[0];
    
    gsmData.txSMS.rcvrNumType = 0x91;                       //Constants
    gsmData.txSMS.pduType = 0x11;
    gsmData.txSMS.msgRef = 0;
    gsmData.txSMS.PID = 0;
    gsmData.txSMS.DCS = 0;
    gsmData.txSMS.VP = 0xAA;
    
    gsmData.txSMS.pduLen = 0;                               //Reset the counter
    
    *pOut = 0;                                              //SCA
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.pduType;
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.msgRef;
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.rcvrNumLen;
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.rcvrNumType;
    pOut++;
    gsmData.txSMS.pduLen++;
    //pRcvrNum = rcvrNumCmp();
    for(i = 0; i < (gsmData.txSMS.rcvrNumLen/2 + gsmData.txSMS.rcvrNumLen%2); i++){
        //*pOut = *pRcvrNum;
        *pOut = swapNibbles(gsmData.txSMS.rcvrNum[i]);
        pOut++;
        //pRcvrNum++;
        gsmData.txSMS.pduLen++;
    }
    *pOut = gsmData.txSMS.PID;
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.DCS;
    pOut++;
    gsmData.txSMS.pduLen++;
    *pOut = gsmData.txSMS.VP;
    pOut++;
    gsmData.txSMS.pduLen++;
    gsmData.txSMS.msgLength = strlen((char *)gsmData.txSMS.message);
    *pOut = gsmData.txSMS.msgLength;
    pOut++;
    gsmData.txSMS.pduLen++;
    gsmDefaultEncode(gsmData.txSMS.message, msgPdu, gsmData.txSMS.msgLength);
    ftmp = (float)gsmData.txSMS.msgLength*7./8.;
    msgLen = (int)ftmp;
    if((ftmp - (int)ftmp) != 0) msgLen++;
    for(i = 0; i < msgLen; i++){
        *pOut = *pMsgPdu;
        pOut++;
        pMsgPdu++;
        gsmData.txSMS.pduLen++;
    }
}

/*!***************************************************************************
* @brief    
* @param    pdu - pointer to PDU data field
*/
void pduDecode(void){
    uint8_t *pdu, cnt, i;//, tmp[6];
    pdu = &gsmData.rxSMS.PDU[0];
    
    cnt = *pdu;                                                                  //Increment the SMSC block
    pdu++;
    pdu++;
    for(i = 0; i < cnt-1; i++){
        pdu++;
    }
    gsmData.rxSMS.pduType = *pdu;                                                //Parse PDU type
    pdu++;
    cnt = *pdu;                                                                  //Parse the sender number block
    gsmData.rxSMS.sndrNumLen = *pdu;
    cnt = cnt/2 + cnt%2;
    pdu++;
    gsmData.rxSMS.sndrNumType = *pdu;
    pdu++;
    for(i = 0; i < cnt; i++){
        gsmData.rxSMS.sndrNum[i] = swapNibbles(*pdu);
        pdu++;
    }
    //sndrNumCmp(tmp);
    gsmData.rxSMS.PID = *pdu;                                                    //Protocol ID
    pdu++;
    gsmData.rxSMS.DCS = *pdu;                                                    //Data coding scheme
    pdu++;
    tm.tm_year = YEAR + bcd2int(swapNibbles(*pdu));
    pdu++;
    tm.tm_mon = bcd2int(swapNibbles(*pdu));
    pdu++;
    tm.tm_mday = bcd2int(swapNibbles(*pdu));
    pdu++;
    tm.tm_hour = bcd2int(swapNibbles(*pdu));
    pdu++;
    tm.tm_min = bcd2int(swapNibbles(*pdu));
    pdu++;
    tm.tm_sec = bcd2int(swapNibbles(*pdu));
    pdu++;
    pdu++;
    gsmData.rxSMS.unixTime = xtmtot();
    gsmData.rxSMS.msgLength = *pdu;
    pdu++;
    gsmDefaultDecode(pdu, gsmData.rxSMS.message, gsmData.rxSMS.msgLength);       //Parse user data block
}

/*!***************************************************************************
* @brief    
* @param    pdu - pointer to user data field
* @param    str - pointer to decode buffer
* @param    len - length of decode string
*/
void gsmDefaultDecode(uint8_t *pdu, uint8_t *str, uint16_t len){
    uint16_t cnt = 0;
    
    *str = (*pdu << (cnt % 8)) & 0x7F;
    str++;
    pdu++;
    
    for(cnt = 1; cnt < len; cnt++){
        *str = (*pdu << (cnt % 8)) & 0x7F;
        *str |= *(pdu - 1) >> (8 - (cnt % 8));
        str++;
        if((((cnt + 1) % 8) != 0)){
            pdu++;
        }
    }
}

uint8_t swapNibbles(uint8_t x){
    return ( (x & 0x0F)<<4 | (x & 0xF0)>>4 );
}

uint8_t bcd2int(uint8_t hex){
    uint8_t dec;
    dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
    return dec;
}

uint16_t int2bcd(uint8_t uiValue){
    uint16_t uiResult = 0, i = 0;
    
    while(uiValue > 0){
        uiResult |= (uiValue % 10)<<4*i;
        uiValue /= 10;
        i++;
    }
    return uiResult;
}
/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

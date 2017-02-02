/*!****************************************************************************
* @file    sysTSK.c
* @author  4eef
* @version V1.0
* @date    04-Oct-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "sysTSK.h"

/*!****************************************************************************
* MEMORY
*/
extern tm_type tm;
extern gsmData_type gsmData;
extern gpsData_type gpsData;

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void sysTSK(void *pPrm){
    uint16_t itmp;
    int16_t i;
    char *tmp;
    
    while(1){
        if((gsmData.gsmStatus.gsmSmsRcvd != 0) && (gsmData.rxSMS.rdngSms == 0) && (gsmData.txSMS.sndngSms == 0)){
            gsmData.gsmStatus.gsmSmsRcvd = 0;
            //If the GPS data is asked
            tmp = string(getGPS);
            itmp = 0;
            for(i = 0; i < strlen(tmp); i++) {                      //Compare with temporary string
                if (tmp[i] == gsmData.rxSMS.message[i]){
                    itmp++;
                }
            }
            if(itmp == strlen(tmp)){
                memset(gsmData.txSMS.message, 0, sizeof(gsmData.txSMS.message));    //Clear fields from previous message
                memset(gsmData.txSMS.rcvrNum, 0, sizeof(gsmData.txSMS.rcvrNum));
                //Receiver address
                gsmData.txSMS.rcvrNumLen = gsmData.rxSMS.sndrNumLen;///2 + gsmData.rxSMS.sndrNumLen%2;
                memcpy(gsmData.txSMS.rcvrNum, gsmData.rxSMS.sndrNum, gsmData.txSMS.rcvrNumLen);
                //Check the GPS and write appropriate message
                if(gpsData.gpsStatus.gpsOn != 0){
                    if(gpsData.precision != 0){
                        memcpy(gsmData.txSMS.message, fillGpsString(), strlen(fillGpsString()));
                    }else{
                        memcpy(gsmData.txSMS.message, string(gpsNotRdy), strlen(string(gpsNotRdy)));
                    }
                }else{
                    memcpy(gsmData.txSMS.message, string(gpsOff), strlen(string(gpsOff)));
                }
                //Flag to send SMS
                gsmData.gsmStatus.gsmSmsTxRdy = 1;
            }
            //If the GPS ON is performed
            tmp = string(gpsOnCmd);
            itmp = 0;
            for(i = 0; i < strlen(tmp); i++) {                      //Compare with temporary string
                if (tmp[i] == gsmData.rxSMS.message[i]){
                    itmp++;
                }
            }
            if(itmp == strlen(tmp)){
                gpsData.gpsStatus.gpsOn = 1;
                gpsData.gpsStatus.gpsSet = 0;
                memset(gsmData.txSMS.message, 0, sizeof(gsmData.txSMS.message));    //Clear fields from previous message
                memset(gsmData.txSMS.rcvrNum, 0, sizeof(gsmData.txSMS.rcvrNum));
                //Receiver address
                gsmData.txSMS.rcvrNumLen = gsmData.rxSMS.sndrNumLen;///2 + gsmData.rxSMS.sndrNumLen%2;
                memcpy(gsmData.txSMS.rcvrNum, gsmData.rxSMS.sndrNum, gsmData.txSMS.rcvrNumLen);
                //Check the GPS and write appropriate message
                memcpy(gsmData.txSMS.message, string(ok), strlen(string(ok)));
                //Flag to send SMS
                gsmData.gsmStatus.gsmSmsTxRdy = 1;
            }
            //If the GPS OFF is performed
            tmp = string(gpsOffCmd);
            itmp = 0;
            for(i = 0; i < strlen(tmp); i++) {                      //Compare with temporary string
                if (tmp[i] == gsmData.rxSMS.message[i]){
                    itmp++;
                }
            }
            if(itmp == strlen(tmp)){
                gpsData.gpsStatus.gpsOn = 0;
                gpsData.gpsStatus.gpsSet = 0;
                memset(gsmData.txSMS.message, 0, sizeof(gsmData.txSMS.message));    //Clear fields from previous message
                memset(gsmData.txSMS.rcvrNum, 0, sizeof(gsmData.txSMS.rcvrNum));
                //Receiver address
                gsmData.txSMS.rcvrNumLen = gsmData.rxSMS.sndrNumLen;///2 + gsmData.rxSMS.sndrNumLen%2;
                memcpy(gsmData.txSMS.rcvrNum, gsmData.rxSMS.sndrNum, gsmData.txSMS.rcvrNumLen);
                //Check the GPS and write appropriate message
                memcpy(gsmData.txSMS.message, string(ok), strlen(string(ok)));
                //Flag to send SMS
                gsmData.gsmStatus.gsmSmsTxRdy = 1;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

/*!****************************************************************************
* @file    gpsTSK.c
* @author  4eef
* @version V1.0
* @date    03-Aug-16
* @brief   --
*/

/*!****************************************************************************
* Include
*/
#include "gpsTSK.h"

/*!****************************************************************************
* MEMORY
*/
uartTsk_type        uartTsk;
static char       *pCommands[] = {
    "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n",  //Zero output
    "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0*29\r\n",  //GPGGA, GPVTG, GPZDA output
    "$PMTK101*32\r\n",                                        //Hot Start
    "$PMTK102*31\r\n",                                        //Warm Start
    "$PMTK103*30\r\n",                                        //Cold Start
    "$PMTK104*37\r\n",                                        //Full Cold Start
    "$PMTK161,0*28\r\n",                                      //StandBy mode
    "$PMTK605*31\r\n",                                        //Get formware info
    "$PMTK705,AXN_2.10_3339_13053100,0000,B02V17SIM28_11,*38\r\n"//Firmware info
};

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
void gps_sendCommand(gpsCommands_enum_type cmd){
    extern char *trxbuff;
    int     len = 0;
    len = strlen(pCommands[cmd]);                                       //Calculate the length
    memset(&trxbuff[0], 0, sizeof(trxbuff));
    memcpy(trxbuff, pCommands[cmd], len);                         //Copy the command to UART buffer
    uart_write(uart2, trxbuff, len);                          //Perform the transfer
    xSemaphoreTake(semUart2Tx, pdMS_TO_TICKS(500));                     //Wait until transfer completes
}

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
uint8_t checkSum(char *str) {
    uint8_t len, check = 0;
    char *lStr = str;
    *lStr++;
    len = strlen(lStr);
    len = len - 5;
    //Iterate over the string, XOR each byte with the total sum
    for(int c = 0; c < len; c++){
        check = check ^ lStr[c];
    }
    return check;
}

/*!****************************************************************************
* @brief   
* @param    
* @retval   
*/
gpsData_type    gpsData;
extern tm_type tm;
static char rxtmp[UART2_RxBffSz], *trxbuff, *pTmp;
    
void gpsTSK(void *pPrm){
    BaseType_t  res;
    uint16_t rxBffSz = UART2_RxBffSz;
    char GPGGA[72], GPVTG[48], GPZDA[40];
    trxbuff = &rxtmp[0];
    gpsData.gpsStatus.gpsError = 0;
    gpsData.gpsStatus.gpsOn = 0;
    gpsData.gpsStatus.gpsSet = 0;
    gpsData.gpsStatus.gpsOk = 0;
    
    //gpsData.gpsStatus.gpsOn = 1;                                                //For debug purposes
    
    while(1){
        uart_read(uart2, uart2->pRxBff, rxBffSz);
        gps_sendCommand(outFmtOff);                                             //Set off the output format
        xSemaphoreTake(semUart2Rx, pdMS_TO_TICKS(1000));
        uart_read(uart2, uart2->pRxBff, rxBffSz);
        gps_sendCommand(getFirmware);
        res = xSemaphoreTake(semUart2Rx, pdMS_TO_TICKS(1000));
        if(res == pdTRUE){
            //Repeat the transfer if empty string is received
            char rcs[2] = {0};
            memcpy(trxbuff, uart2->pRxBff, rxBffSz);
            uint8_t csum = checkSum(trxbuff);                                    //Calculate the checksum
            trxbuff += 9;                                                        //Remove the "$PMTK###," string
            uint8_t len = strlen(trxbuff);
            rcs[0] = trxbuff[len-4];                                             //Copy received checksum
            rcs[1] = trxbuff[len-3];
            uint8_t rxCSum = (unsigned char)strtol(rcs, (char**)NULL, 16);
            memset(&gpsData.moduleInfo[0], 0, sizeof(gpsData.moduleInfo));      //Zero the chars array
            if(rxCSum == csum){                                                 //Verify the checksum
                memcpy(gpsData.moduleInfo, trxbuff, len-5);                     //Copy received module info to struct
                gpsData.gpsStatus.gpsOk = 1;
                break;
            }else{
                gpsData.gpsStatus.gpsError = 1;                                 //Checksum isn't matched
            };
        }else{
            gpsData.gpsStatus.gpsError = 1;                                     //No data received
        };
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1));
        if(gpsData.gpsStatus.gpsOn != 0){
            if(gpsData.gpsStatus.gpsSet != 0){
                //receive the packets
                uart_read(uart2, uart2->pRxBff, rxBffSz);
                res = xSemaphoreTake(semUart2Rx, pdMS_TO_TICKS(2000));
                if(res == pdTRUE){
                    //Divide received packet on sub-packets
                    trxbuff = &rxtmp[0];                                        //Prepare arrays
                    memset(trxbuff, 0, sizeof(trxbuff));
                    memcpy(trxbuff, uart2->pRxBff, rxBffSz);
                    pTmp = &GPGGA[0];
                    memset(pTmp, 0, sizeof(GPGGA));
                    pTmp = &GPVTG[0];
                    memset(pTmp, 0, sizeof(GPVTG));
                    pTmp = &GPZDA[0];
                    memset(pTmp, 0, sizeof(GPZDA));
                    divideStr(GPGGA, GPVTG, GPZDA, rxtmp);                      //Divide received packet
                    //Parse packets
                    float lat, lon, ftmp;
                    int itmp;
                    char ctmp;
                    if(gpsData.gpsStatus.gpsOk != 0){
                        //GPGGA packet parsing
                        sscanf(GPGGA, "$GPGGA,%2d%2d%2d.%3d,%f,%c,%f,%c,%d,%d,%f,%f,%c", &itmp, &itmp, &itmp, &itmp, &lat, &gpsData.lat_sign, &lon, &gpsData.lon_sign, &gpsData.precision, &gpsData.numSats, &gpsData.HDOP, &gpsData.altitude, &gpsData.altUnit);
                        gpsData.lat_degrees = (int)(lat/100.0);
                        lat = lat - gpsData.lat_degrees*100;
                        gpsData.lat_minutes = (int)lat;
                        gpsData.lat_seconds = (lat - gpsData.lat_minutes)*60;
                        if(gpsData.lat_minutes > 60){
                            gpsData.lat_degrees++;
                            gpsData.lat_minutes -= 60;
                        }
                        gpsData.lon_degrees = (int)(lon/100.0);
                        lon = lon - gpsData.lon_degrees*100;
                        gpsData.lon_minutes = (int)lon;
                        gpsData.lon_seconds = (lon - gpsData.lon_minutes)*60;
                        if(gpsData.lon_minutes > 60){
                            gpsData.lon_degrees++;
                            gpsData.lon_minutes -= 60;
                        }
                        //GPVTG packet parsing
                        sscanf(GPVTG, "$GPVTG,%f,%c,%f,%c,%f,%c,%f,%c", &ftmp, &ctmp, &ftmp, &ctmp, &gpsData.speedKph, &ctmp, &gpsData.speedKnots, &ctmp);
                        //GPZDA packet parsing
                        sscanf(GPZDA, "$GPZDA,%2d%2d%2d.%3d,%d,%d,%d,%d,%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec, &itmp, &tm.tm_mday, &tm.tm_mon, &tm.tm_year, &itmp, &itmp);
                        gpsData.unixTime = xtmtot();
                    }
                }
            }else{
                uart_read(uart2, uart2->pRxBff, rxBffSz);
                gps_sendCommand(fullColdStart);                                 //Set the full cold start
                BaseType_t res2 = xSemaphoreTake(semUart2Rx, 1000);
                uart_read(uart2, uart2->pRxBff, rxBffSz);
                gps_sendCommand(outFmt);                                        //Set the output format
                BaseType_t res1 = xSemaphoreTake(semUart2Rx, 1000);
                if((res1 && res2) == pdTRUE) gpsData.gpsStatus.gpsSet = 1;
            }
        }else{
            if(gpsData.gpsStatus.gpsSet == 0){
                uart_read(uart2, uart2->pRxBff, rxBffSz);
                gps_sendCommand(standBy);                                        //Set off the module
                BaseType_t res = xSemaphoreTake(semUart2Rx, 1000);
                if(res == pdTRUE) gpsData.gpsStatus.gpsSet = 1;
            }
        }
    }
}

//String dividing routine
void divideStr(char *GPGGA, char *GPVTG, char *GPZDA, char *rxtmp){
    uint8_t i = 0, j = 0;
    char *tmp, *pTmp;
    
    do{
        GPGGA[i] = rxtmp[j];
        i++;
        j++;
    } while (rxtmp[j] != '\r');
    tmp = "GPGGA";
    for(uint8_t ii = 0; ii <= 4; ii++) {
        if (tmp[ii] != GPGGA[ii+1]){
            pTmp = &GPGGA[0];
            memset(pTmp, 0, sizeof(GPGGA));
            gpsData.gpsStatus.gpsOk = 0;
            return;
        }
    }
    i = 0;
    j++;
    j++;
    
    do{
        GPVTG[i] = rxtmp[j];
        i++;
        j++;
    } while (rxtmp[j] != '\r');
    tmp = "GPVTG";
    for(uint8_t ii = 0; ii <= 4; ii++) {
        if (tmp[ii] != GPVTG[ii+1]){
            pTmp = &GPVTG[0];
            memset(pTmp, 0, sizeof(GPVTG));
            gpsData.gpsStatus.gpsOk = 0;
            return;
        }
    }
    i = 0;
    j++;
    j++;
    
    do{
        GPZDA[i] = rxtmp[j];
        i++;
        j++;
    } while (rxtmp[j] != '\r');
    tmp = "GPZDA";
    for(uint8_t ii = 0; ii <= 4; ii++) {
        if (tmp[ii] != GPZDA[ii+1]){
            pTmp = &GPZDA[0];
            memset(pTmp, 0, sizeof(GPZDA));
            gpsData.gpsStatus.gpsOk = 0;
            return;
        }
    }
    gpsData.gpsStatus.gpsOk = 1;
}

/***************** (C) COPYRIGHT ************** END OF FILE ******** 4eef ****/

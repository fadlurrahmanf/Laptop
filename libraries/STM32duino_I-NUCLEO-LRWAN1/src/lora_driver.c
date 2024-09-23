/*******************************************************************************
 * @file    Lora_driver.c based on V1.1.2
 * @author  MCD Application Team
 * @brief   LoRa module API
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "hw.h"
#include "lora_driver.h"
#include "tiny_sscanf.h"

/* External variables --------------------------------------------------------*/
/*
 * Global flag to treat the return value of Lora_GetFWVersion() function
 * which is the only one that is not preceded by '=' charater.
 * This flag is used in the at_cmd_receive(..) function
 */
ATCmd_t gFlagException = AT_END_AT;
/*
 * Depending of firmware version AT+VERB is supported or not.
 * Introduced with fw 2.8, since this, some AT response format have changed.
 * This boolean will allow to handle those format.
 */
bool AT_VERB_cmd = true;

/* Private typedef -----------------------------------------------------------*/

/* Private variable ----------------------------------------------------------*/
/*
 * To get back the device address in 11:22:33:44 format before
 * to be translated into uint32_t type
 */
static uint8_t PtrValueFromDevice[32];
/* In relation with the response size */
static uint8_t PtrTempValueFromDeviceKey[64];
/* Payload size max returned by USI modem */
static uint8_t PtrDataFromNetwork[128];

/* Private define ------------------------------------------------------------*/

/******************************************************************************/
/*                    To put USI modem in sleep mode                          */
/* From USI FW V2.6, modem sleep mode is only supported on ABP Join mode      */
/* From USI FW V3.0, modem sleep mode is supported for ABP and OTAA Join mode */
/******************************************************************************/

/* Private functions ---------------------------------------------------------*/
static void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size );

/**************************************************************
 * @brief  Check if the LoRa module is working
 * @param  void
 * @retval status of the module (ready or not ready)
**************************************************************/
RetCode_t Lora_Init(void)
{
  ATEerror_t Status;

  /* Check if the module is working */
  Status = Modem_AT_Cmd(AT_CTRL, AT, NULL);

  if (Status == AT_OK) {
    /* Received Ok from module */
    return MODULE_READY;
  } else {
    return MODULE_NO_READY;
  }
}

/**************************************************************
 * @brief  reset of the LoRa module
 * @param  void
 * @retval void
**************************************************************/
void Lora_Reset(void)
{
  /* Reset the lora module */
  Modem_AT_Cmd(AT_CTRL, AT_RESET, NULL);
}

/**************************************************************
 * @brief  Do a request to establish a LoRa Connection with the gateway
 * @param  Mode: by OTAA or by ABP
 * @retval LoRA return code
 * @Nota param is relevant for USI WM_SG_SM_XX modem - Not relevant for MDM32L07X01 modem
**************************************************************/
ATEerror_t Lora_Join(uint8_t Mode)
{
  ATEerror_t Status = AT_END_ERROR;

  /******************************************************************/
  /* In OTAA mode wait JOIN_ACCEPT_DELAY1 cf. LoRaWAN Specification */
  /* MDM32L07X01:                                                   */
  /*      - After Join request waits DELAY_FOR_JOIN_STATUS_REQ      */
  /*      - Then do Join status request to know is nwk joined       */
  /* WM_SG_SM_XX:                                                   */
  /*      - Do the Join request                                     */
  /*      - Then waits synchronous JoinAccept event                 */
  /*      - if timeout raised before JoinAccept event               */
  /*      - then Join request Failed                                */
  /* Nota: Lora_Join() does the join request                        */
  /*       afterwhat                                                */
  /*       Lora_JoinAccept() does the waiting on return event       */
  /******************************************************************/

  switch(Mode) {
    case ABP_JOIN_MODE:
      /* Request a join connection */
      Status = Modem_AT_Cmd(AT_SET, AT_JOIN, &Mode);
      break;
    case OTAA_JOIN_MODE:
      Status = Modem_AT_Cmd(AT_SET, AT_JOIN, &Mode);
      /* HW_EnterSleepMode( );*/
      if(Status == AT_OK)
      {
        Status = AT_JOIN_SLEEP_TRANSITION;  /* to go in low power mode idle loop*/
      }
      break;
    default:
      break;
  }
  return Status;
}

/**************************************************************
 * @brief  Wait for join accept notification either in ABP or OTAA
 * @param  void
 * @retval LoRA return code
 * @Nota this function supports either USI protocol or MDM32L07X01 protocol
**************************************************************/
ATEerror_t Lora_JoinAccept(void)
{
  /* Trap the asynchronous accept event (OTAA mode) coming from USI modem */
  return Modem_AT_Cmd(AT_ASYNC_EVENT, AT_JOIN, NULL);
}

/**************************************************************
 * @brief  Do a request to set the Network join Mode
 * @param  Mode : OTA, ABP
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetJoinMode(uint8_t Mode)
{
  /* Set the nwk Join mode */
  return Modem_AT_Cmd(AT_SET, AT_NJM, &Mode);
}

/**************************************************************
 * @brief  Do a request to get the Network join Mode
 * @param  pointer to the Join mode out value
 * @retval LoRa return code
*************************************************************                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                FF_SIZE -1;
    }
    memcpy1(PtrStructData->Buffer, (uint8_t *)&PtrValueFromDevice[3], sizebuf+1);
  }
  return Status;
}

/**************************************************************
 * @brief  Trap an asynchronous event coming from external modem (only USI device)
 * @param  Pointer to RCV out value if any
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_AsyncDownLinkData(sReceivedDataBinary_t *PtrStructData)
{
  ATEerror_t Status;
  uint8_t sizebuf;
  char *ptrChr;

  Status = Modem_AT_Cmd(AT_ASYNC_EVENT, AT_END_AT, PtrDataFromNetwork);
  if (Status == 0) {
    AT_VSSCANF((char*)PtrDataFromNetwork, "%d,%2d",
               &(PtrStructData->Port),&(PtrStructData->DataSize));
    /* Search the last ',' occurence in the return string */
    ptrChr = strrchr((char*)&PtrDataFromNetwork[0], ',')+1;
	if ((sizebuf=strlen((char*)ptrChr)) > DATA_RX_MAX_BUFF_SIZE) {
      /* Shrink the Rx buffer to MAX size */
      sizebuf = DATA_RX_MAX_BUFF_SIZE -1;
    }
    /* Prevent a memory overflow in case of corrupted read */
    if(sizebuf == 0) {
      return AT_TEST_PARAM_OVERFLOW;
    }
    memcpy1(PtrStructData->Buffer, (uint8_t *)ptrChr, sizebuf);
    *(PtrStructData->Buffer+sizebuf) ='\0';
  }
  return Status;
}

/**************************************************************
 * @brief  Send binary data to a giving port number
 * @param  SENDB in value ( USE_MDM32L07X01) SEND in value ( USE_I_NUCLEO_LRWAN1)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SendDataBin(sSendDataBinary_t *PtrStructData)
{
  /* Remove all old data in the uart rx buffer to prevent response issue */
  HW_UART_Modem_Flush();
  return Modem_AT_Cmd(AT_SET, AT_SEND, PtrStructData);
}

/**************************************************************
 * @brief  Do a request to get the last data (in binary format)
 * @brief  received by the Slave
 * @param  pointer to RECVB out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_ReceivedDataBin(sReceivedDataBinary_t *PtrStructData)
{
  ATEerror_t Status;
  uint8_t sizebuf;
  uint8_t i;
  char TempBuf[3] ={0};

  Status = Modem_AT_Cmd(AT_GET, AT_RECVB, PtrValueFromDevice);
  if (Status == 0) {
    AT_VSSCANF((char*)PtrValueFromDevice, "%d",&(PtrStructData->Port));

    if ((sizebuf= strlen((char*)&PtrValueFromDevice[3])) > DATA_RX_MAX_BUFF_SIZE) {
      /* Shrink the Rx buffer to MAX size */
      sizebuf = DATA_RX_MAX_BUFF_SIZE;
    }
    for(i=0; i<=((sizebuf/2)-1); i++) {
      TempBuf[0] = PtrValueFromDevice[3+(i*2)];
      TempBuf[1] = PtrValueFromDevice[3+(i*2)+1];
      AT_VSSCANF(TempBuf,"%hhx", &PtrStructData->Buffer[i]);
    }
    PtrStructData->DataSize = i;
  }
  return Status;
}

/**************************************************************
 * @brief  Do a request to set the confirmation mode
 * @param  CFM in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetSendMsgConfirm(uint8_t ConfirmMode)
{
  return Modem_AT_Cmd(AT_SET, AT_CFM, &ConfirmMode);
}

/**************************************************************
 * @brief  Do a request to get the confirmation mode
 * @param  pointer to CFM out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgConfirm(uint8_t *ConfirmMode)
{
  return Modem_AT_Cmd(AT_GET, AT_CFM, ConfirmMode);
}

/**************************************************************
 * @brief  Do a request to get the msg status of the last send cmd
 * @param  CFS in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgStatus(uint8_t *MsgStatus)
{
  return Modem_AT_Cmd(AT_GET, AT_CFS, MsgStatus);
}

/**************************************************************
 * @brief  Do a request to get the battery level of the modem (slave)
 * @param  BAT in value
 *              0:    battery connected to external power supply
 *       [1..254]:    1 being at minimum and 254 being at maximum
*             255:    not able to measure the battery level
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetBatLevel(uint32_t *BatLevel)
{
  ATEerror_t Status;

  Status = Modem_AT_Cmd(AT_GET, AT_BAT, PtrValueFromDevice);
  if (Status == 0) {
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld", BatLevel);
  }
  return Status;
}

/**************************************************************
 * @brief  Do a request to get the RSSI of the last received packet
 * @param  RSSI in value [in dbm]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetRSSI(int32_t *SigStrengthInd)
{
  ATEerror_t Status;

  Status = Modem_AT_Cmd(AT_GET, AT_RSSI, PtrValueFromDevice);
  if (Status == 0) {
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld", SigStrengthInd);
  }
  return (Status);
}


/**************************************************************
 * @brief  Do a request to get the SNR of the last received packet
 * @param  SNR in value [in db]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSNR(uint32_t *SigToNoice)
{
  ATEerror_t Status;

  Status = Modem_AT_Cmd(AT_GET, AT_SNR, PtrValueFromDevice);
  if (Status == 0) {
    AT_VSSCANF((char*)PtrValueFromDevice, "%ld", SigToNoice);
  }
  return Status;
}

/**************************************************************
 * @brief  Do a request to get the LoRa stack version of the modem (slave)
 * @param  pointer to VER out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetVersion(uint8_t *PtrVersion)
{
  ATEerror_t Status;
  char *ptrChr;

  Status = Modem_AT_Cmd(AT_GET, AT_VER, PtrValueFromDevice);
  if (Status == 0) {
    if(AT_VERB_cmd) {
      /* Skip fw version and model */
      ptrChr = strchr((char *)&PtrValueFromDevice[0], ',');
      strncpy((char*)PtrVersion, (char *)PtrValueFromDevice,
              ptrChr - (char *)PtrValueFromDevice);
	} else {
      /* Skip "LoRaWAN v" */
      ptrChr = strchr((char *)&PtrValueFromDevice[0], 'v');
      strcpy((char*)PtrVersion,ptrChr+1);
    }
  }
  return Status;
}

/**************************************************************
 * @brief  Do a request to get the firmware version of the modem (slave)
 * @param  pointer to FWVERSION out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetFWVersion(uint8_t *PtrFWVersion)
{
  ATEerror_t Status;
  char *ptrChr;

  gFlagException = AT_FWVERSION;
  Status = Modem_AT_Cmd(AT_GET, AT_FWVERSION, PtrValueFromDevice);
  if (Status == 0) {
    /* Skip "USI Lora Module Firmware V" prefix */
    ptrChr = strchr((char *)&PtrValueFromDevice[0],'V');
    strcpy((char*)PtrFWVersion,ptrChr+1);
  } else {
    /*
     * Since AT+VERB introduced, Fw version is integrated to AT+VER
     * <lrwan_ver>,<fw_ver>
     * ATI is marked as removed but always available.
     */
    if(AT_VERB_cmd) {
      Status = Modem_AT_Cmd(AT_GET, AT_VER, PtrValueFromDevice);
      if (Status == 0) {
        char *ptrChr2;
        /* Skip LoRaWan version and strip model */
        ptrChr = strchr((char *)&PtrValueFromDevice[0], ',');
        ptrChr2 = strchr(ptrChr+1,',');
        if (ptrChr2 == NULL) {
          strcpy((char*)PtrFWVersion, ptrChr+1);
        } else {
          strncpy((char*)PtrFWVersion, ptrChr+1, ptrChr2 - ptrChr - 1);
          PtrFWVersion[ptrChr2 - ptrChr - 1] = '\0';
        }
      }
    }
  }
  return Status;
}

/**************************************************************
 * @brief  Do a request to set the country band code for LoRaWAN
 * @brief  Need to write to DCT and Reset module to enable this setting
 * @param  BAND in value 0(EU-868 Band) / 1(US-Band)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDeviceBand(uint8_t DeviceBand)
{
  return Modem_AT_Cmd(AT_SET, AT_BAND, &DeviceBand);
}

/**************************************************************
 * @brief  Do a request to get the country band code for LoRaWAN
 * @brief  only used in test mode
 * @param  pointer to BAND out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDeviceBand(uint8_t *DeviceBand)
{
  return Modem_AT_Cmd(AT_GET, AT_BAND, DeviceBand);
}

/************ Power Control Commands (for USI board) ***************/

/**************************************************************
 * @brief  Do a request to enter the slave in sleep (MCU STOP mode)
 * @param  Void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SleepMode(void)
{
  /* Under building*/
  return Modem_AT_Cmd(AT_EXCEPT_1, AT_SLEEP, PtrValueFromDevice);
}

/**************************************************************
 * @brief  Wait for mcu is going to sleep or is waked up
 * @param  void
 * @retval LoRA return code
**************************************************************/
ATEerror_t Lora_SleepStatus(void)
{
  /* Trap the asynchronous accept event coming from USI modem */
  return Modem_AT_Cmd(AT_ASYNC_EVENT, AT, NULL);
}

/**************************************************************
 * @brief  Do a request to set the power control settings of the MCU (slave)
 * @param  Power control IN value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetMCUPowerCtrl(sPowerCtrlSet_t *PtrStructData)
{
  return Modem_AT_Cmd(AT_SET, AT_PS, PtrStructData);
}

/**************************************************************
 * @brief  Do a Dumy request to resynchronize the Host and the modem
 * @note   A simple AT cmd where we do not trap the return code
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_DumyRequest(void)
{
  ATEerror_t Status;
  uint8_t i;

  for (i=0; i<=1; i++)
  {
    /* First iteration to wake-up the modem */
    /* Ssecond iteration to align Host-Modem interface */
    Status = Modem_AT_Cmd(AT_EXCEPT_1, AT, NULL);
    /* Assumption: to be sure that modem is ready */
    delay(1000);
  }
  return(Status);
}

/**************************************************************
 * @brief  Do a request to restore DCT content table with default values
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_RestoreConfigTable(void)
{
  uint8_t Restore = 0;

  return Modem_AT_Cmd(AT_SET, AT_WDCT, &Restore);
}

/**************************************************************
 * @brief  Do a request to update the DCT content table with new values
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_UpdateConfigTable(void)
{
  return Modem_AT_Cmd(AT_GET, AT_WDCT, NULL );
}

/**************************************************************
 * @brief  memory copy n bytes from src to dst
 * @param  destination pointer
 * @param  source pointer
 * @param  number of bytes to copy
 * @retval none
**************************************************************/
void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
  while( size-- ) {
    *dst++ = *src++;
  }
}

/**************************************************************
 * @brief  Convert keys from char to uint8_t
 * @param  Key to convert.
 * @param  Key converted.
 * @param  length of the integer key.
 **************************************************************/
void keyCharToInt(const char *cKey, uint8_t *iKey, uint8_t length)
{
  if((cKey != NULL) && (iKey != NULL)) {
    char c[3] = {'\0'};
    uint8_t p = 0;
    for(uint8_t i = 0; i < length; i++) {
      c[0] = cKey[p];
      c[1] = cKey[p+1];
      iKey[i] = (uint8_t)strtoul(c, NULL, 16);
      p += 2;
    }
  }
}

/**************************************************************
 * @brief  Convert keys from uint8_t to char
 * @param  Key converted.
 * @param  Key to convert.
 * @param  length of the integer key.
 **************************************************************/
void keyIntToChar(char *cKey, const uint8_t *iKey, uint8_t length)
{
  if((cKey != NULL) && (iKey != NULL)) {
    uint8_t p = 0;
    for(uint8_t i = 0; i < length; i++) {
      itoa(iKey[i], &cKey[p], 16);
      /* Add missing '0' */
      if(cKey[p+1] == '\0') {
        cKey[p+1] = cKey[p];
        cKey[p] = '0';
      }
      p += 2;
    }
  }
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

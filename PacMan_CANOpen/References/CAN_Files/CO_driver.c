/*
   CAN module object for generic microcontroller.

   This file is a template for other microcontrollers.

   @file        CO_driver.c
   @ingroup     CO_driver
   @author      Janez Paternoster
   @copyright   2004 - 2020 Janez Paternoster

   This file is part of CANopenNode, an opensource CANopen Stack.
   Project home page is <https://github.com/CANopenNode/CANopenNode>.
   For more information on CANopen see <http://www.can-cia.org/>.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "CO_driver.h"
#include "CO_Emergency.h"


//extern const CO_CANbitRateData_t  CO_CANbitRateData[8];
//can_timing_config_t t_config;
//can_general_config_t g_config;
//can_filter_config_t f_config;

/******************************************************************************/
void CO_CANsetConfigurationMode(void *CANdriverState) {
  /* Put CAN module in configuration mode */

  //Initialize configuration structures using macro initializers
  //g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, CAN_MODE_NORMAL);
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule) {
  /* Put CAN module in normal mode */
  //g_config.mode = CAN_MODE_NORMAL;
  CANmodule->CANnormal = true;
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
  CO_CANmodule_t         *CANmodule,
  void                   *CANdriverState,
  CO_CANrx_t              rxArray[],
  uint16_t                rxSize,
  CO_CANtx_t              txArray[],
  uint16_t                txSize,
  uint16_t                CANbitRate)
{
  uint16_t i;

  /* verify arguments */
  if (CANmodule == NULL || rxArray == NULL || txArray == NULL) {
    return CO_ERROR_ILLEGAL_ARGUMENT;
  }

  /* Configure object variables */
  CANmodule->CANdriverState = CANdriverState;
  CANmodule->rxArray = rxArray;
  CANmodule->rxSize = rxSize;
  CANmodule->txArray = txArray;
  CANmodule->txSize = txSize;
  CANmodule->CANnormal = false;
  CANmodule->useCANrxFilters = (rxSize <= 32U) ? true : false;/* microcontroller dependent */
  CANmodule->bufferInhibitFlag = false;
  CANmodule->firstCANtxMessage = true;
  CANmodule->CANtxCount = 0U;
  CANmodule->errOld = 0U;
  CANmodule->em = NULL;
 
  for (i = 0U; i < rxSize; i++) {
    rxArray[i].ident = 0U;
    rxArray[i].mask = (uint16_t)0xFFFFFFFFU;
    rxArray[i].object = NULL;
    rxArray[i].pFunct = NULL;
  }
   printf("hello %u ", rxArray[0].ident);
  for (i = 0U; i < txSize; i++) {
    txArray[i].bufferFull = false;
  }


  /* Configure CAN module registers */


  /* Configure CAN timing */
  can_timing_config_t t_config = CAN_TIMING_CONFIG_125KBITS();
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
  can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_25, GPIO_NUM_26, CAN_MODE_NORMAL);

  /* Configure CAN module hardware filters */
  if (CANmodule->useCANrxFilters) {
    can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
    /* CAN module filters are used, they will be configured with */
    /* CO_CANrxBufferInit() functions, called by separate CANopen */
    /* init functions. */
    /* Configure all masks so, that received message must match filter */
  }
  else {
    /* CAN module filters are not used, all messages with standard 11-bit */
    /* identifier will be received */
    /* Configure mask 0 so, that all messages with standard identifier are accepted */
  }

  //Install CAN driver
  esp_err_t can_d_err = can_driver_install(&g_config, &t_config, &f_config);

  if (can_d_err == ESP_OK) {
    printf("Driver installed\n");
  } else if (can_d_err == ESP_ERR_INVALID_ARG) {
    printf("Failed to install driver\n");
    return CO_ERROR_ILLEGAL_ARGUMENT;
  } else if (can_d_err == ESP_ERR_NO_MEM) {
    printf("Failed to install driver\n");
    return CO_ERROR_OUT_OF_MEMORY;
  } else if (can_d_err == ESP_ERR_INVALID_STATE) {
    printf("Failed to install driver\n");
    return CO_ERROR_INVALID_STATE;
  } else {
    printf("Failed to install driver\n");
    return CO_ERROR_SYSCALL;
  }

  //Start CAN driver
  if (can_start() == ESP_OK) {
    printf("Driver started\n");
  } else {
    printf("Failed to start driver\n");
    return CO_ERROR_INVALID_STATE;
  }

  /* configure CAN interrupt registers */


  return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule) {
  /* turn off the module */
  CO_CANsetConfigurationMode(CANmodule->CANdriverState);
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg) {
  return (uint16_t) rxMsg->ident;
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
  CO_CANmodule_t         *CANmodule,
  uint16_t                index,
  uint16_t                ident,
  uint16_t                mask,
  bool_t                  rtr,
  void                   *object,
  void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message))
{
  CO_ReturnError_t ret = CO_ERROR_NO;

  if ((CANmodule != NULL) && (object != NULL) && (pFunct != NULL) && (index < CANmodule->rxSize)) {
    /* buffer, which will be configured */
    CO_CANrx_t *buffer = &CANmodule->rxArray[index];

    /* Configure object variables */
    buffer->object = object;
    buffer->pFunct = pFunct;

    /* CAN identifier and CAN mask, bit aligned with CAN module. Different on different microcontrollers. */
    buffer->ident = ident & 0x07FFU;
    if (rtr) {
      buffer->ident |= 0x0800U;
    }
    buffer->mask = (mask & 0x07FFU) | 0x0800U;

    /* Set CAN hardware module filter and mask. */
    if (CANmodule->useCANrxFilters) {

    }
  }
  else {
    ret = CO_ERROR_ILLEGAL_ARGUMENT;
  }

  return ret;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
  CO_CANmodule_t         *CANmodule,
  uint16_t                index,
  uint16_t                ident,
  bool_t                  rtr,
  uint8_t                 noOfBytes,
  bool_t                  syncFlag)
{
  CO_CANtx_t *buffer = NULL;

  if ((CANmodule != NULL) && (index < CANmodule->txSize)) {
    /* get specific buffer */
    buffer = &CANmodule->txArray[index];

    /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer.
       Microcontroller specific. */

    buffer->esp_ident = ((uint32_t)ident & 0x07FFU); // Get original ID for later
    buffer->ident = ((uint32_t)ident & 0x07FFU)
                    | ((uint32_t)(((uint32_t)noOfBytes & 0xFU) << 12U))
                    | ((uint32_t)(rtr ? 0x8000U : 0U));
    buffer->bufferFull = false;
    buffer->syncFlag = syncFlag;
  }

  return buffer;
}


/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer) {
  CO_ReturnError_t err = CO_ERROR_NO;

  /* Verify overflow */
  if (buffer->bufferFull) {
    if (!CANmodule->firstCANtxMessage) {
      /* don't set error, if bootup message is still on buffers */
      CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, buffer->ident);
    }
    err = CO_ERROR_TX_OVERFLOW;
  }
  // Starting to create message to use the ESP IDF Send Function
  can_message_t message;
  message.identifier = buffer->esp_ident;
  message.extd = 0;
  message.data_length_code = CAN_FRAME_MAX_DLC;

  for (int i = 0; i < CAN_FRAME_MAX_DLC; i++) {
    message.data[i] = buffer->data[i];
    printf("\nMessage Print: %u ", message.data[i]);
  }
  CO_LOCK_CAN_SEND();
  /* if CAN TX buffer is free, copy message to it */
  if (1 && CANmodule->CANtxCount == 0) {
    CANmodule->bufferInhibitFlag = buffer->syncFlag;

    //Queue message for transmission
    if (can_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
      printf("Message queued for transmission\n");
    } else {
      printf("Failed to queue message for transmission\n");
    }
  }
  /* if no buffer is free, message will be sent by interrupt */
  else {
    buffer->bufferFull = true;
    CANmodule->CANtxCount++;
  }
  CO_UNLOCK_CAN_SEND();

  return err;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule) {
  uint32_t tpdoDeleted = 0U;

  CO_LOCK_CAN_SEND();
  /* Abort message from CAN module, if there is synchronous TPDO.
     Take special care with this functionality. */
  if (/*messageIsOnCanBuffer && */CANmodule->bufferInhibitFlag) {
    /* clear TXREQ */
    CANmodule->bufferInhibitFlag = false;
    tpdoDeleted = 1U;
  }
  /* delete also pending synchronous TPDOs in TX buffers */
  if (CANmodule->CANtxCount != 0U) {
    uint16_t i;
    CO_CANtx_t *buffer = &CANmodule->txArray[0];
    for (i = CANmodule->txSize; i > 0U; i--) {
      if (buffer->bufferFull) {
        if (buffer->syncFlag) {
          buffer->bufferFull = false;
          CANmodule->CANtxCount--;
          tpdoDeleted = 2U;
        }
      }
      buffer++;
    }
  }
  CO_UNLOCK_CAN_SEND();


  if (tpdoDeleted != 0U) {
    CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_TPDO_OUTSIDE_WINDOW, CO_EMC_COMMUNICATION, tpdoDeleted);
  }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule) {
  uint16_t rxErrors, txErrors, overflow;
  CO_EM_t* em = (CO_EM_t*)CANmodule->em;
  uint32_t err;

  /* get error counters from module. Id possible, function may use different way to
     determine errors. */
  rxErrors = CANmodule->txSize;
  txErrors = CANmodule->txSize;
  overflow = CANmodule->txSize;

  err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;

  if (CANmodule->errOld != err) {
    CANmodule->errOld = err;

    if (txErrors >= 256U) {                             /* bus off */
      CO_errorReport(em, CO_EM_CAN_TX_BUS_OFF, CO_EMC_BUS_OFF_RECOVERED, err);
    }
    else {                                              /* not bus off */
      CO_errorReset(em, CO_EM_CAN_TX_BUS_OFF, err);

      if ((rxErrors >= 96U) || (txErrors >= 96U)) {   /* bus warning */
        CO_errorReport(em, CO_EM_CAN_BUS_WARNING, CO_EMC_NO_ERROR, err);
      }

      if (rxErrors >= 128U) {                         /* RX bus passive */
        CO_errorReport(em, CO_EM_CAN_RX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
      }
      else {
        CO_errorReset(em, CO_EM_CAN_RX_BUS_PASSIVE, err);
      }

      if (txErrors >= 128U) {                         /* TX bus passive */
        if (!CANmodule->firstCANtxMessage) {
          CO_errorReport(em, CO_EM_CAN_TX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
        }
      }
      else {
        bool_t isError = CO_isError(em, CO_EM_CAN_TX_BUS_PASSIVE);
        if (isError) {
          CO_errorReset(em, CO_EM_CAN_TX_BUS_PASSIVE, err);
          CO_errorReset(em, CO_EM_CAN_TX_OVERFLOW, err);
        }
      }

      if ((rxErrors < 96U) && (txErrors < 96U)) {     /* no error */
        CO_errorReset(em, CO_EM_CAN_BUS_WARNING, err);
      }
    }

    if (overflow != 0U) {                               /* CAN RX bus overflow */
      CO_errorReport(em, CO_EM_CAN_RXB_OVERFLOW, CO_EMC_CAN_OVERRUN, err);
    }
  }
}


/******************************************************************************/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule) {

  can_message_t RXmessage; // For ESP Receiving
  /* receive interrupt */
  printf("waiting for a message!\n");
  esp_err_t receiveStatus = can_receive(&RXmessage, pdMS_TO_TICKS(10000)); // Call the receive method for CAN packets

  if (receiveStatus == ESP_OK) {
    printf("a message is being processed!\n");
    CO_CANrxMsg_t *rcvMsg;      /* pointer to received message in CAN module */
    uint16_t index;             /* index of received message */
    uint32_t rcvMsgIdent;       /* identifier of the received message */
    CO_CANrx_t *buffer = NULL;  /* receive message buffer from CO_CANmodule_t object. */
    bool_t msgMatched = false;

    CO_CANrxMsg_t CANOpenMsg;
    rcvMsg = &CANOpenMsg; /* get message from module here */
    CANOpenMsg.ident = RXmessage.identifier;
    CANOpenMsg.DLC = RXmessage.data_length_code;
    for (int i = 0; i < CAN_FRAME_MAX_DLC; i++)
      CANOpenMsg.data[i] = RXmessage.data[i];

    rcvMsg = &CANOpenMsg; /* get message from module here */
    rcvMsgIdent = CO_CANrxMsg_readIdent(rcvMsg);
    
    if (CANmodule->useCANrxFilters) {
      printf("can is filtered\n");
      /* CAN module filters are used. Message with known 11-bit identifier has */
      /* been received */
      index = 8;  /* get index of the received message here. Or something similar */
      if (index < CANmodule->rxSize) {
        buffer = &CANmodule->rxArray[index];
        /* verify also RTR */
        printf("rxsize: %u ", CANmodule->rxSize);
        printf("ident: %u ", buffer->ident);
        printf("Mask: %u ", buffer->mask);
        printf("MSg: %u ", rcvMsgIdent);
        if (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) {
          printf("got a msg\n");
          msgMatched = true;
        }
      }
    }
    else {
      /* CAN module filters are not used, message with any standard 11-bit identifier */
      /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
      printf("can is not filtered\n");
      for (index = CANmodule->rxSize; index > 0U; index--) {
              buffer = &CANmodule->rxArray[index];
        if (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) {
          msgMatched = true;
          break;
        }
        buffer++;
      }
    }

    /* Call specific function, which will process the message */
    if (msgMatched && (buffer != NULL) && (buffer->pFunct != NULL)) {
      printf("Called specific function \n");
      buffer->pFunct(buffer->object, rcvMsg);
    }

    /* Clear interrupt flag */
    // DW
  }


  /* transmit interrupt */
  else if (receiveStatus == ESP_ERR_TIMEOUT || receiveStatus == ESP_ERR_INVALID_ARG) {
    /* Clear interrupt flag */
    printf("Timeout or Invalid Flag!");

    /* First CAN message (bootup) was sent successfully */
    CANmodule->firstCANtxMessage = false;
    /* clear flag from previous message */
    CANmodule->bufferInhibitFlag = false;
    /* Are there any new messages waiting to be send */
    if (CANmodule->CANtxCount > 0U) {
      uint16_t i;             /* index of transmitting message */

      /* first buffer */
      CO_CANtx_t *buffer = &CANmodule->txArray[0];
      /* search through whole array of pointers to transmit message buffers. */
      for (i = CANmodule->txSize; i > 0U; i--) {
        /* if message buffer is full, send it. */
        if (buffer->bufferFull) {
          buffer->bufferFull = false;
          CANmodule->CANtxCount--;

          /* Copy message to CAN buffer */
          CANmodule->bufferInhibitFlag = buffer->syncFlag;

          /* canSend... */
          can_message_t TXmessage;
          TXmessage.identifier = buffer->esp_ident;
          TXmessage.extd = 0;
          TXmessage.data_length_code = buffer->DLC;
          for (int i = 0; i < CAN_FRAME_MAX_DLC; i++)
            TXmessage.data[i] = buffer->data[i];

          //Queue message for transmission
          if (can_transmit(&TXmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
            printf("Message queued for transmission\n");
          } else {
            printf("Failed to queue message for transmission\n");
          }

          break;                      /* exit for loop */
        }
        buffer++;
      }/* end of for loop */

      /* Clear counter if no more messages */
      if (i == 0U) {
        CANmodule->CANtxCount = 0U;
      }
    }
  }
  else {
    /* some other interrupt reason */
  }
}

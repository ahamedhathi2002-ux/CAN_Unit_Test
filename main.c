#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "components.h"
#include "can_lld_cfg.h"
#include "can_transmit.h"          /* <-- replaces the old static function */

#ifdef __CDT_PARSER__
typedef unsigned char  uint8_t;
typedef unsigned long  uint32_t;
#ifndef bool
typedef unsigned char bool;
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif
#endif

static uint32_t counter;

/* comment this define and remove the callback field
 * in the configurator if you want to use the can_lld_receive
 * instead of the read callback routines
 */
#define USE_READ_CALLBACK

/* Use this define and remove the callback field
 * in the configurator if you want to use the can_lld_receive
 * instead of the read callback routines
 */
//#define USE_READ_FUNCTION

/* mcanconf is configured to use also CANFD operation
 * uncommenting following define will allow test application
 * to send and receive CANFD frames on MCAN SUB 0 CAN 1
 */
//#define USE_CANFD

void mcanconf_errorcb(CANDriver *canp, uint32_t psr) {
  (void)canp;
  (void)psr;
}

#if defined USE_READ_CALLBACK
void mcanconf_fiforx(uint32_t msgbuf, CANRxFrame crfp) {
#ifdef USE_CANFD
  if (crfp.OPERATION == CAN_OP_CANFD) {
#endif
    if (crfp.ID == 0x7f0U) {
      if (crfp.data32[1] == counter) {
        pal_lld_togglepad(PORT_E, LED_4);
      }
    }
#ifdef USE_CANFD
  }
#endif
  (void)msgbuf;
}

void mcanconf1_rxreceive(uint32_t msgbuf, CANRxFrame crfp) {
  if (crfp.ID == 0x8901234UL) {
    if (crfp.data32[1] == counter) {
      pal_lld_togglepad(PORT_A, LED_3);
    }
  }
  (void)msgbuf;
}
#endif /* USE_READ_CALLBACK */

int main(void) {

  uint8_t vehicleTxData[8U];
  uint8_t chargerTxData[8U];
  CAN_Status_t transmitStatus;
#if defined USE_READ_FUNCTION
  uint32_t returnvalue;
#endif

  componentsInit();
  irqIsrEnable();

  can_lld_start(&CAND2, &can_config_mcanconf);   /* MCAN SUB 0 CAN 1 */
  can_lld_start(&CAND7, &can_config_mcanconf1);  /* MCAN SUB 1 CAN 1 */

  counter = 0UL;

  for (;;) {

    (void)memset(vehicleTxData, 0, sizeof(vehicleTxData));
    (void)memset(chargerTxData, 0, sizeof(chargerTxData));

    (void)memcpy(&vehicleTxData[0], &(uint32_t){0xDDEEFFAAUL}, sizeof(uint32_t));
    (void)memcpy(&vehicleTxData[4], &counter,                   sizeof(counter));
    (void)memcpy(&chargerTxData[0], &(uint32_t){0xAABBCCDDUL}, sizeof(uint32_t));
    (void)memcpy(&chargerTxData[4], &counter,                   sizeof(counter));

    transmitStatus = CAN_Transmit(0x7f0U, vehicleTxData, sizeof(vehicleTxData),
                                  true, CAN_CHANNEL_VEHICLE);
    osalThreadDelayMilliseconds(1);
    if (transmitStatus != CAN_SUCCESS) {
      for (;;) { }
    }

    transmitStatus = CAN_Transmit(0x8901234UL, chargerTxData, sizeof(chargerTxData),
                                  false, CAN_CHANNEL_CHARGER);
    osalThreadDelayMilliseconds(1);
    if (transmitStatus != CAN_SUCCESS) {
      for (;;) { }
    }

#if defined USE_READ_FUNCTION
    {
      CANRxFrame rxf;

      returnvalue = can_lld_receive(&CAND2, CAN_FIFO0_RXBUFFER, &rxf);
#ifdef USE_CANFD
      if (rxf.OPERATION == CAN_OP_CANFD) {
#endif
        if (returnvalue == CAN_MSG_OK) {
          if (rxf.ID == 0x7f0U) {
            if (rxf.data32[1] == counter) {
              pal_lld_togglepad(PORT_E, LED_4);
            }
          }
        }
#ifdef USE_CANFD
      }
#endif

      returnvalue = can_lld_receive(&CAND7, CAN_ANY_RXBUFFER, &rxf);
      if (rxf.ID == 0x8901234UL) {
        if (rxf.data32[1] == counter) {
          pal_lld_togglepad(PORT_A, LED_3);
        }
      }
    }
#endif /* USE_READ_FUNCTION */

    osalThreadDelayMilliseconds(250);
    counter++;
  }
}
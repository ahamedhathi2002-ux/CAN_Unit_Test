/**
 * can_transmit.c
 *
 * CAN transmit module — extracted from main.c so it can be unit-tested.
 * On the real firmware build this pulls in the real SDK headers.
 * On the test build the Makefile adds -DUNIT_TEST and sdk_stubs.h is
 * included via the compiler -include flag (see Makefile).
 */

#include <string.h>
#include <stddef.h>
#include "can_transmit.h"

/* ------------------------------------------------------------------ */
/* In the real firmware build these headers come from the SDK.         */
/* In the test build sdk_stubs.h is force-included by the Makefile.   */
/* ------------------------------------------------------------------ */
#ifndef UNIT_TEST
#  include "can_lld_cfg.h"
#  include "components.h"
#else
#  include "sdk_stubs.h"   /* types and constants for test build */
#endif

/* ------------------------------------------------------------------ */
/* Driver and sizing constants                                          */
/* ------------------------------------------------------------------ */
#define CAN_MAX_DATA_LENGTH     SPC5_CAN_MAX_DATA_LENGHT   /* SDK spelling */
#define CAN_TX_MAX_RETRY_COUNT  3U
#define VEHICLE_CAN_DRIVER      (&CAND2)
#define CHARGER_CAN_DRIVER      (&CAND7)

/* ------------------------------------------------------------------ */

CAN_Status_t CAN_Transmit(const uint32_t ID,
                           const uint8_t *p_Data,
                           const uint8_t  len,
                           const bool     b_isStandard,
                           const CAN_Channel_t canChannel)
{
    CANTxFrame  canFrame;
    CANDriver  *p_canDriver = NULL;
    uint8_t     sendTryCount = 0U;
    uint32_t    state = CAN_MSG_WAIT;

    /* ---- guard: length ------------------------------------------- */
    if ((len == 0U) || (len > CAN_MAX_DATA_LENGTH)) {
        return CAN_ERR_LENGTH;
    }

    /* ---- guard: null pointer ------------------------------------- */
    if (p_Data == NULL) {
        return CAN_ERR_INVALID_DATA;
    }

    /* ---- build frame --------------------------------------------- */
    memset(&canFrame, 0, sizeof(canFrame));
    canFrame.DLC  = len;
    canFrame.ID   = ID;
    canFrame.TYPE = b_isStandard ? CAN_ID_STD : CAN_ID_XTD;

    /* ---- select driver ------------------------------------------- */
    switch (canChannel) {
        case CAN_CHANNEL_VEHICLE:
            p_canDriver = VEHICLE_CAN_DRIVER;
            break;

        case CAN_CHANNEL_CHARGER:
            p_canDriver = CHARGER_CAN_DRIVER;
            break;

        default:
            return CAN_ERR_INVALID_CHANNEL;
    }

    memcpy(canFrame.data8, p_Data, len);

    /* ---- transmit with retry ------------------------------------- */
    for (sendTryCount = 0U; sendTryCount < CAN_TX_MAX_RETRY_COUNT; ++sendTryCount) {
        state = can_lld_transmit(p_canDriver, CAN_ANY_TXBUFFER, &canFrame);
        if (state == CAN_MSG_OK) {
            break;
        }
    }

    /* ---- map hw state to status ---------------------------------- */
    if (state == CAN_MSG_WAIT) {
        return CAN_BUSY;
    } else if (state != CAN_MSG_OK) {
        return CAN_ERR_TIMEOUT;
    } else {
        return CAN_SUCCESS;
    }
}
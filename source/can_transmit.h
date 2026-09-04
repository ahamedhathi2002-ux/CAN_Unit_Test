/**
 * can_transmit.h
 *
 * Public interface for the CAN transmit module.
 * Include this in both main.c and test code.
 */

#ifndef CAN_TRANSMIT_H
#define CAN_TRANSMIT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CAN_SUCCESS = 0,
    CAN_BUSY,
    CAN_ERR_LENGTH,
    CAN_ERR_INVALID_DATA,
    CAN_ERR_INVALID_CHANNEL,
    CAN_ERR_TIMEOUT
} CAN_Status_t;

typedef enum {
    CAN_CHANNEL_VEHICLE = 0,
    CAN_CHANNEL_CHARGER
} CAN_Channel_t;

CAN_Status_t CAN_Transmit(uint32_t       ID,
                           const uint8_t *p_Data,
                           uint8_t        len,
                           bool           b_isStandard,
                           CAN_Channel_t  canChannel);

#endif /* CAN_TRANSMIT_H */
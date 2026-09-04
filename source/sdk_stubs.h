#ifndef SDK_STUBS_H
#define SDK_STUBS_H

#include <stdint.h>

#define CAN_ID_STD   0U
#define CAN_ID_XTD   1U

#define CAN_MSG_OK    0U
#define CAN_MSG_WAIT  1U

#define CAN_ANY_TXBUFFER  0xFFFFFFFFU   /* now uint32_t, see below */

#define SPC5_CAN_MAX_DATA_LENGHT 8U

typedef struct {
    uint32_t ID;
    uint8_t  DLC;
    uint8_t  TYPE;
    uint8_t  data8[8];
} CANTxFrame;

typedef struct {
    uint8_t id;   /* <-- required: mock.c initializes CAND2 = { .id = 2U } */
} CANDriver;

extern CANDriver CAND2;
extern CANDriver CAND7;

/* mailbox param is uint32_t in mock.c, not uint8_t — must match exactly */
uint32_t can_lld_transmit(CANDriver  *canp,
                           uint32_t    mailbox,
                           CANTxFrame *ctfp);

#endif /* SDK_STUBS_H */
/**
 * can_lld_transmit_mock.c
 *
 * Implements:
 *   - can_lld_transmit()  (replaces the real SDK function at link time)
 *   - CAND2, CAND7 global stubs (replaces SDK-generated globals)
 *   - Mock control API
 */

#include "can_lld_transmit_mock.h"
#include <string.h>

/* ------------------------------------------------------------------ */
/* Global driver stubs — mimic what the SDK normally exports           */
/* ------------------------------------------------------------------ */
CANDriver CAND2 = { .id = 2U };
CANDriver CAND7 = { .id = 7U };

/* ------------------------------------------------------------------ */
/* Internal mock state                                                  */
/* ------------------------------------------------------------------ */
static uint32_t   mock_sequence[MOCK_MAX_SEQ];
static uint8_t    mock_count;
static uint8_t    mock_call_index;
static uint8_t    call_count;
static CANDriver *last_driver;
static CANTxFrame last_frame;

/* ------------------------------------------------------------------ */
/* Control API                                                          */
/* ------------------------------------------------------------------ */

void CAN_LldTransmitMock_Reset(void)
{
    memset(mock_sequence, 0, sizeof(mock_sequence));
    mock_count      = 0U;
    mock_call_index = 0U;
    call_count      = 0U;
    last_driver     = NULL;
    memset(&last_frame, 0, sizeof(last_frame));
}

void CAN_LldTransmitMock_SetReturnSequence(const uint32_t *sequence,
                                           uint8_t         count)
{
    uint8_t n = (count > MOCK_MAX_SEQ) ? MOCK_MAX_SEQ : count;
    memcpy(mock_sequence, sequence, n * sizeof(uint32_t));
    mock_count      = n;
    mock_call_index = 0U;
}

uint8_t CAN_LldTransmitMock_GetCallCount(void)
{
    return call_count;
}

CANDriver *CAN_LldTransmitMock_GetLastDriver(void)
{
    return last_driver;
}

CANTxFrame CAN_LldTransmitMock_GetLastFrame(void)
{
    return last_frame;
}

/* ------------------------------------------------------------------ */
/* Replacement for the real can_lld_transmit()                         */
/* ------------------------------------------------------------------ */

uint32_t can_lld_transmit(CANDriver *canp, uint32_t mailbox, CANTxFrame *ctfp)
{
    (void)mailbox;

    last_driver = canp;

    if (ctfp != NULL) {
        last_frame = *ctfp;
    }

    call_count++;

    if (mock_call_index < mock_count) {
        return mock_sequence[mock_call_index++];
    }

    /* Default: bus is busy — safe for tests that forgot to set a sequence */
    return CAN_MSG_WAIT;
}
/**
 * can_lld_transmit_mock.h
 *
 * Test-side replacement for the real can_lld_transmit() hardware call.
 * Link this instead of the SDK object in test builds.
 */

#ifndef CAN_LLD_TRANSMIT_MOCK_H
#define CAN_LLD_TRANSMIT_MOCK_H

#include <stdint.h>
#include "sdk_stubs.h"   /* CANDriver, CANTxFrame */

/* Maximum number of return values you can queue per test */
#define MOCK_MAX_SEQ 8U

/**
 * Reset all state — call in setUp().
 */
void CAN_LldTransmitMock_Reset(void);

/**
 * Queue the values that successive can_lld_transmit() calls will return.
 * Once the sequence is exhausted every further call returns CAN_MSG_WAIT.
 *
 * @param sequence  Array of CAN_MSG_OK / CAN_MSG_WAIT / other values.
 * @param count     Number of entries (max MOCK_MAX_SEQ).
 */
void CAN_LldTransmitMock_SetReturnSequence(const uint32_t *sequence,
                                           uint8_t         count);

/**
 * How many times was can_lld_transmit() called since the last Reset()?
 */
uint8_t CAN_LldTransmitMock_GetCallCount(void);

/**
 * Which CANDriver pointer was passed on the most recent call?
 */
CANDriver *CAN_LldTransmitMock_GetLastDriver(void);

/**
 * Copy of the CANTxFrame passed on the most recent call.
 * Lets tests inspect DLC, ID, TYPE, data8.
 */
CANTxFrame CAN_LldTransmitMock_GetLastFrame(void);

#endif /* CAN_LLD_TRANSMIT_MOCK_H */
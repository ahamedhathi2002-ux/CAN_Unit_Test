/**
 * test_can_transmit.c
 *
 * Unity test suite for CAN_Transmit().
 *
 * Groups:
 *   1. Length guard
 *   2. Null-pointer guard
 *   3. Invalid channel guard
 *   4. Happy path — first-try success
 *   5. Driver routing (VEHICLE → CAND2, CHARGER → CAND7)
 *   6. Frame fields (ID, DLC, TYPE, data)
 *   7. Retry logic
 *   8. Return-status mapping (BUSY, TIMEOUT)
 */

#include "unity.h"
#include "can_transmit.h"
#include "can_lld_transmit_mock.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static const uint8_t DUMMY_DATA[8] = {0x01, 0x02, 0x03, 0x04,
                                       0x05, 0x06, 0x07, 0x08};

/* ------------------------------------------------------------------ */
/* setUp / tearDown — run before/after every test                       */
/* ------------------------------------------------------------------ */

void setUp(void)
{
    CAN_LldTransmitMock_Reset();
}

void tearDown(void)
{
    /* nothing needed — mock is reset in setUp */
}

/* ==================================================================
 * GROUP 1 — Length guard
 * ================================================================== */

void test_CAN_Transmit_ShouldReturn_ErrLength_WhenLenIsZero(void)
{
    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 0U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_ERR_LENGTH, status);
    /* Guard must fire before any HW call */
    TEST_ASSERT_EQUAL_UINT8(0U, CAN_LldTransmitMock_GetCallCount());
}

void test_CAN_Transmit_ShouldReturn_ErrLength_WhenLenIs65(void)
{
    /* Classic CAN max is 8, CAN FD max is 64 — 65 is always over */
    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 65U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_ERR_LENGTH, status);
    TEST_ASSERT_EQUAL_UINT8(0U, CAN_LldTransmitMock_GetCallCount());
}

void test_CAN_Transmit_ShouldAccept_MaxValidLength(void)
{
    /* 8-byte payload matches the current classic CAN limit */
    uint8_t big[8];
    uint8_t i;
    for (i = 0U; i < 8U; i++) { big[i] = i; }

    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Status_t status = CAN_Transmit(0x7F0U, big, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_SUCCESS, status);
}

/* ==================================================================
 * GROUP 2 — NULL pointer guard
 * ================================================================== */

void test_CAN_Transmit_ShouldReturn_ErrInvalidData_WhenDataIsNull(void)
{
    CAN_Status_t status = CAN_Transmit(0x7F0U, NULL, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_ERR_INVALID_DATA, status);
    TEST_ASSERT_EQUAL_UINT8(0U, CAN_LldTransmitMock_GetCallCount());
}

/* ==================================================================
 * GROUP 3 — Invalid channel guard
 * ================================================================== */

void test_CAN_Transmit_ShouldReturn_ErrInvalidChannel_WhenChannelIs99(void)
{
    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, (CAN_Channel_t)99U);

    TEST_ASSERT_EQUAL(CAN_ERR_INVALID_CHANNEL, status);
    TEST_ASSERT_EQUAL_UINT8(0U, CAN_LldTransmitMock_GetCallCount());
}

/* ==================================================================
 * GROUP 4 — Happy path
 * ================================================================== */

void test_CAN_Transmit_ShouldReturn_Success_WhenHwAcksOnFirstTry(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_SUCCESS, status);
}

void test_CAN_Transmit_ShouldCallHw_ExactlyOnce_OnFirstTrySuccess(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL_UINT8(1U, CAN_LldTransmitMock_GetCallCount());
}

/* ==================================================================
 * GROUP 5 — Driver routing
 * ================================================================== */

void test_CAN_Transmit_ShouldUse_CAND2_ForVehicleChannel(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL_PTR(&CAND2, CAN_LldTransmitMock_GetLastDriver());
}

void test_CAN_Transmit_ShouldUse_CAND7_ForChargerChannel(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x8901234UL, DUMMY_DATA, 8U, false, CAN_CHANNEL_CHARGER);

    TEST_ASSERT_EQUAL_PTR(&CAND7, CAN_LldTransmitMock_GetLastDriver());
}

/* ==================================================================
 * GROUP 6 — Frame field verification
 * ================================================================== */

void test_CAN_Transmit_ShouldSet_StandardID_WhenFlagIsTrue(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    CANTxFrame frame = CAN_LldTransmitMock_GetLastFrame();
    TEST_ASSERT_EQUAL_UINT8(CAN_ID_STD, frame.TYPE);
}

void test_CAN_Transmit_ShouldSet_ExtendedID_WhenFlagIsFalse(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x8901234UL, DUMMY_DATA, 8U, false, CAN_CHANNEL_CHARGER);

    CANTxFrame frame = CAN_LldTransmitMock_GetLastFrame();
    TEST_ASSERT_EQUAL_UINT8(CAN_ID_XTD, frame.TYPE);
}

void test_CAN_Transmit_ShouldSet_CorrectDLC_InFrame(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    CANTxFrame frame = CAN_LldTransmitMock_GetLastFrame();
    TEST_ASSERT_EQUAL_UINT8(8U, frame.DLC);
}

void test_CAN_Transmit_ShouldSet_CorrectID_InFrame(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    CANTxFrame frame = CAN_LldTransmitMock_GetLastFrame();
    TEST_ASSERT_EQUAL_UINT32(0x7F0U, frame.ID);
}

void test_CAN_Transmit_ShouldCopy_DataBytes_IntoFrame(void)
{
    uint32_t seq[] = {CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 1U);

    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    CANTxFrame frame = CAN_LldTransmitMock_GetLastFrame();
    TEST_ASSERT_EQUAL_UINT8_ARRAY(DUMMY_DATA, frame.data8, 8U);
}

/* ==================================================================
 * GROUP 7 — Retry logic
 * ================================================================== */

void test_CAN_Transmit_ShouldRetry_AndReturn_Success_OnSecondAttempt(void)
{
    uint32_t seq[] = {CAN_MSG_WAIT, CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 2U);

    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_SUCCESS, status);
    TEST_ASSERT_EQUAL_UINT8(2U, CAN_LldTransmitMock_GetCallCount());
}

void test_CAN_Transmit_ShouldRetry_AndReturn_Success_OnThirdAttempt(void)
{
    uint32_t seq[] = {CAN_MSG_WAIT, CAN_MSG_WAIT, CAN_MSG_OK};
    CAN_LldTransmitMock_SetReturnSequence(seq, 3U);

    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_SUCCESS, status);
    TEST_ASSERT_EQUAL_UINT8(3U, CAN_LldTransmitMock_GetCallCount());
}

void test_CAN_Transmit_ShouldStop_AtExactly3Retries_WhenAllWait(void)
{
    /* No sequence set → mock always returns CAN_MSG_WAIT */
    CAN_Transmit(0x7F0U, DUMMY_DATA, 8U, true, CAN_CHANNEL_VEHICLE);

    /* Must not loop more than CAN_TX_MAX_RETRY_COUNT times */
    TEST_ASSERT_EQUAL_UINT8(3U, CAN_LldTransmitMock_GetCallCount());
}

/* ==================================================================
 * GROUP 8 — Return-status mapping
 * ================================================================== */

void test_CAN_Transmit_ShouldReturn_Busy_WhenAllRetriesReturnWait(void)
{
    /* Default mock behaviour → always CAN_MSG_WAIT */
    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_BUSY, status);
}

void test_CAN_Transmit_ShouldReturn_Timeout_WhenHwReturnsUnknownError(void)
{
    uint32_t seq[] = {0xDEADU, 0xDEADU, 0xDEADU};
    CAN_LldTransmitMock_SetReturnSequence(seq, 3U);

    CAN_Status_t status = CAN_Transmit(0x7F0U, DUMMY_DATA, 8U,
                                        true, CAN_CHANNEL_VEHICLE);

    TEST_ASSERT_EQUAL(CAN_ERR_TIMEOUT, status);
}

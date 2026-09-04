This application demonstrates how to send and receive can message using MCAN.

Two CAN configured:
- MCAN subsystem 0 CAN 1 --> CANFD enabled, FIFO0 with one STD filter 
- MCAN subsystem 1 CAN 1 --> NORMAL CAN, Dedicated RX buffer with one XTD filter

Using loopback features, each time a message is received a led is toggled (one for MCAN0_1 and one for MCAN1_1)

Read could be performed in two ways:
- using can_lld_read function
- using callback function defined in each configuration created

If you want to test CANFD operation on MCAN Sub 0 CAN 1 please uncomment define USE_CANFD in main.c 

By default the app uses callbacks. if you want to use read functions instead
 - comment #define USE_READ_CALLBACK 
 - uncomment #define USE_READ_FUNCTION
 - remove rx buffer callbacks and disable interrupts in both configurations (mcanconf and mcanconf1)
 
For mcanconf, an error callback is defined. It will be called when one of the following error occurs:
 *  - WDI:  Watchdog Interrupt
 *  - BO:   BusOff
 *  - EW:   Warning Status
 *  - EP:   Error Passive
 *  - ELO:  Error Logging Overflow
 *  - BEU:  Bit Error Uncorrected
 *  - BEC:  Bit Error Corrected
 *  - TOO:  Timeout
 *  - ARA:  Access to reserved address
 *  - PED:  Protocol Error In Data Phase
 *  - PEA:  Protocol error in Arbitration Phase
 *  - MRAF: Message RAM access Failure
 
 MCAN_ERROR_MASK defined in platform.h contains bit map for these errors. refer to RM if you want to change errors trapped.
 
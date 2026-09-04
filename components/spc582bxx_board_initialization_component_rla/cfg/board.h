/****************************************************************************
*
* Copyright © 2017-2022 STMicroelectronics - All Rights Reserved
*
* This software is licensed under SLA0098 terms that can be found in the
* DM00779817_1_0.pdf file in the licenses directory of this software product.
* 
* THIS SOFTWARE IS DISTRIBUTED "AS IS," AND ALL WARRANTIES ARE DISCLAIMED, 
* INCLUDING MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*****************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

#include "pal.h"

/*
 * Setup for a generic SPC582Bxx board.
 */

/*
 * Board identifiers.
 */
#define BOARD_SPC582B_DISCOVERY
#define BOARD_NAME                  "STMicroelectronics SPC582B Discovery"

/*
 * Support macros.
 */
#define MSCR_IO_INDEX(port, pin)    (((port) * 16U) + (pin))

/*
 * I/O definitions.
 */
#define LIN1_RXD                    1U
#define LED_3                       11U
#define LED_5                       5U
#define LED_4                       11U

/*
 * PORT definitions.
 */
#define PORT_LIN1_RXD               PORT_A
#define PORT_LED_3                  PORT_A
#define PORT_LED_5                  PORT_D
#define PORT_LED_4                  PORT_E

/*
 * MSCR_IO definitions.
 */
#define MSCR_IO_LIN1_RXD            MSCR_IO_INDEX(PORT_LIN1_RXD, LIN1_RXD)
#define MSCR_IO_LED_3               MSCR_IO_INDEX(PORT_LED_3, LED_3)
#define MSCR_IO_LED_5               MSCR_IO_INDEX(PORT_LED_5, LED_5)
#define MSCR_IO_LED_4               MSCR_IO_INDEX(PORT_LED_4, LED_4)

/*
 * MSCR_MUX definitions.
 */
#define MSCR_MUX_LIN1_RXD           337U

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */

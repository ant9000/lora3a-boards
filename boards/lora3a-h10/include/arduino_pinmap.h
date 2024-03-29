/*
 * Copyright (C)  2018 Federico Pellegrin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_saml21-xpro
 * @brief       Mapping from board pins to Arduino pins
 * @file
 * @author      Federico Pellegrin <fede@evolware.org>
 * @{
 */

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

#include "board.h"
#include "periph_cpu.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Arduino's digital pins mappings
 */
#define ARDUINO_PIN_0   BTN0_PIN
#define ARDUINO_PIN_1   GPIO_PIN(PB, 31) /* SX1276 - NSS      */
#define ARDUINO_PIN_2   GPIO_PIN(PB, 15) /*          RESET    */
#define ARDUINO_PIN_3   GPIO_PIN(PB, 16) /*          DIO0     */
#define ARDUINO_PIN_4   GPIO_PIN(PA, 11) /*          DIO1     */
#define ARDUINO_PIN_5   GPIO_PIN(PA, 12) /*          DIO2     */
#define ARDUINO_PIN_6   GPIO_PIN(PB, 17) /*          DIO3     */
#define ARDUINO_PIN_7   GPIO_PIN(PA, 13) /*          BAND_SEL */

/**
 * @brief   Arduino's analog pins mappings
 */
#define ARDUINO_A0      ADC_LINE(0)
#define ARDUINO_A1      ADC_LINE(1)

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_PINMAP_H */
/** @} */

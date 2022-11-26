#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Arduino's digital pins mappings
 */
static const gpio_t arduino_pinmap[] = {
    ARDUINO_PIN_0
};

/**
 * @brief   Arduino's analog pins mappings
 */
static const adc_t arduino_analog_map[] = {
    ARDUINO_A0,
    ARDUINO_A1
};

/**
 * @brief   On-board LED mapping
 */
#define ARDUINO_LED         (0)

/**
 * @brief   On-board serial port mapping
 */
#define ARDUINO_UART_DEV         UART_DEV(0)

#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_BOARD_H */
/** @} */

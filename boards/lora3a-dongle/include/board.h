#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SX127X_PARAMS  { .spi       = (SPI_DEV(0)),               \
                         .nss_pin   = GPIO_PIN(PB, 31), /* D10 */ \
                         .reset_pin = GPIO_PIN(PB, 15), /* A0 */  \
                         .dio0_pin  = GPIO_PIN(PB, 16), /* D2 */  \
                         .dio1_pin  = GPIO_PIN(PA, 11), /* D3 */  \
                         .dio2_pin  = GPIO_PIN(PA, 12), /* D4 */  \
                         .dio3_pin  = GPIO_PIN(PB, 17), /* D5 */  \
                         .paselect  = (SX127X_PA_RFO) }

#define BTN0_PIN     GPIO_PIN(PA, 19)
#define BTN0_MODE    GPIO_IN_PU

#define RGBLED_RED   GPIO_PIN(PA, 13)
#define RGBLED_GREEN GPIO_PIN(PA,  7)
#define RGBLED_BLUE  GPIO_PIN(PA,  6)

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */

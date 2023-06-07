#ifndef BOARD_H
#define BOARD_H

#include "sx127x.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SX127X_PARAM_PASELECT
// use SX127X_PA_RFO for no boost
#define SX127X_PARAM_PASELECT   SX127X_PA_BOOST
#endif

#define SX127X_PARAMS  { .spi       = (SPI_DEV(0)),               \
                         .nss_pin   = GPIO_PIN(PB, 31), /* D10 */ \
                         .reset_pin = GPIO_PIN(PB, 15), /* A0 */  \
                         .dio0_pin  = GPIO_PIN(PB, 16), /* D2 */  \
                         .dio1_pin  = GPIO_PIN(PA, 11), /* D3 */  \
                         .dio2_pin  = GPIO_PIN(PA, 12), /* D4 */  \
                         .dio3_pin  = GPIO_PIN(PB, 17), /* D5 */  \
                         .paselect  = (SX127X_PARAM_PASELECT) }

#define TCXO_PWR_PIN        GPIO_PIN(PA, 9)     /**< 32 MHz TCXO 1ppm oscillator for radio enable */
#define TX_OUTPUT_SEL_PIN   GPIO_PIN(PA, 13)    /**< BAND_SEL */

#define BTN0_PIN       GPIO_PIN(PA, 6)
#ifndef BTN0_MODE
#define BTN0_MODE      GPIO_IN_PU
#endif

#define I2C0_ENABLE_PIN     GPIO_PIN(PA, 27)

#define HDC3020_ENABLE_PIN  I2C0_ENABLE_PIN
#define HDC3020_PARAM_I2C   (I2C_DEV(0))

#define FRAM_ENABLE_PIN     I2C0_ENABLE_PIN
#define FRAM_PARAM_I2C      (I2C_DEV(0))

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */

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
                         .paselect  = (SX127X_PA_BOOST) }    

#define BTN0_PIN       GPIO_PIN(PA, 7)
#define BTN0_MODE      GPIO_IN_PU

//#define SX127X_PARAM_PASELECT               SX127X_PA_RFO         /**< no boost */
#define SX127X_PARAM_PASELECT               SX127X_PA_BOOST         /**< boost */

#define TCXO_PWR_PIN   GPIO_PIN(PA, 9)     /**< 32 MHz TCXO 1ppm oscillator for radio enable */
#define TX_OUTPUT_SEL_PIN	GPIO_PIN(PA, 13)    /**< BAND_SEL */


//#define LED_PORT       PORT->Group[0]

//#define LED0_PIN       GPIO_PIN(PA, 13)
//#define LED0_MASK      (1 << 13)
//#define LED0_ON        (LED_PORT.OUTCLR.reg = LED0_MASK)
//#define LED0_OFF       (LED_PORT.OUTSET.reg = LED0_MASK)
//#define LED0_TOGGLE    (LED_PORT.OUTTGL.reg = LED0_MASK)

//#define LED1_PIN       GPIO_PIN(PA, 7)
//#define LED1_MASK      (1 << 7)
//#define LED1_ON        (LED_PORT.OUTCLR.reg = LED1_MASK)
//#define LED1_OFF       (LED_PORT.OUTSET.reg = LED1_MASK)
//#define LED1_TOGGLE    (LED_PORT.OUTTGL.reg = LED1_MASK)

//#define LED2_PIN       GPIO_PIN(PA, 6)
//#define LED2_MASK      (1 << 6)
//#define LED2_ON        (LED_PORT.OUTCLR.reg = LED2_MASK)
//#define LED2_OFF       (LED_PORT.OUTSET.reg = LED2_MASK)
//#define LED2_TOGGLE    (LED_PORT.OUTTGL.reg = LED2_MASK)


void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */

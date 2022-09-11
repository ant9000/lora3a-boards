#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#endif

void board_init(void)
{
    /* initialize the CPU */
//    cpu_init();

/* add pullups to UART0 pins */
    PORT->Group[PA].PINCFG[22].bit.PULLEN = 1;
    PORT->Group[PA].PINCFG[23].bit.PULLEN = 1;

    /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
    gpio_init(TCXO_PWR_PIN, GPIO_OUT);
    gpio_clear(TCXO_PWR_PIN);
    gpio_init(TX_OUTPUT_SEL_PIN, GPIO_OUT);
    gpio_write(TX_OUTPUT_SEL_PIN, !SX127X_PARAM_PASELECT);
#endif /* USEMODULE_SX127X */

#if defined(BME688_ACME1)
	puts("BME688 on ACME Sensor 1\n");
	#define BME688_ENABLE  GPIO_PIN(PA, 28)
#else
    // disable i2c port on acme sensor 1
    gpio_init(GPIO_PIN(PB, 2), GPIO_IN_PU);
    gpio_init(GPIO_PIN(PB, 3), GPIO_IN_PU);
#endif
#if defined(BME688_ACME2)
	puts("BME688 on ACME Sensor 2\n");
	#define BME688_ENABLE  GPIO_PIN(PA, 31)
#else
    // disable i2c port on acme sensor 2
    gpio_init(GPIO_PIN(PA, 4), GPIO_IN_PU);
    gpio_init(GPIO_PIN(PA, 5), GPIO_IN_PU);
#endif


}

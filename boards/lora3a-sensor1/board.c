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
    cpu_init();

    /* add pullups to UART0 pins */
    PORT->Group[PA].PINCFG[22].bit.PULLEN = 1;
    PORT->Group[PA].PINCFG[23].bit.PULLEN = 1;

    /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
#endif /* USEMODULE_SX127X */
}

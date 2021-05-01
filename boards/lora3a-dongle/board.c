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

    /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
#endif /* USEMODULE_SX127X */
}

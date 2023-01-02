#include <stdio.h>
#include "saml21_cpu_debug.h"
#include "saml21_backup_mode.h"

#include "sx127x_internal.h"
#include "sx127x_params.h"
#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/i2c.h"
#include "periph/uart.h"

#define WAKEUP_PIN 6  /* from 0 = PA00 to 7 = PA07; -1 to disable; BTN_0 is 6 */
#define SLEEP_TIME 5 /* in seconds; -1 to disable */

void sensors_init(void)
{
    puts("Sensors init.");
}

void sensors_read(void)
{
    puts("Sensors read.");
}

void lora_send_command(void)
{
    puts("Lora send command.");
}

void periodic_task(void)
{
    puts("Periodic task.");
}

void poweroff_devices(void)
{
    size_t i;

    // turn radio off
    sx127x_t sx127x;
    sx127x.params = sx127x_params[0];
    spi_init(sx127x.params.spi);
#ifdef TCXO_PWR_PIN
    gpio_set(TCXO_PWR_PIN);
#endif
    sx127x_init(&sx127x);
    sx127x_reset(&sx127x);
    sx127x_set_sleep(&sx127x);
#ifdef TCXO_PWR_PIN
    gpio_clear(TCXO_PWR_PIN);
#endif
#ifdef TX_OUTPUT_SEL_PIN
    gpio_clear(TX_OUTPUT_SEL_PIN);
#endif

    // turn I2C devices off (leave internal bus I2C_DEV(0) alone)
    for(i = 1; i < I2C_NUMOF; i++) {
        i2c_release(I2C_DEV(i));
        i2c_deinit_pins(I2C_DEV(i));
        gpio_init(i2c_config[i].scl_pin, GPIO_IN_PU);
        gpio_init(i2c_config[i].sda_pin, GPIO_IN_PU);
    }

    saml21_cpu_debug();
}

int main(void)
{
    switch(saml21_wakeup_cause()) {
        case BACKUP_EXTWAKE:
            sensors_read();
            lora_send_command();
            break;
        case BACKUP_RTC:
            periodic_task();
            break;
        default:
            if (WAKEUP_PIN > -1) {
                printf("GPIO PA%d will wake the board.\n", WAKEUP_PIN);
            }
            if (SLEEP_TIME > -1) {
                printf("Periodic task running every %d seconds.\n", SLEEP_TIME);
            }
            sensors_init();
            break;
    }

    puts("Entering backup mode.");
    poweroff_devices();
    saml21_backup_mode_enter(WAKEUP_PIN, SLEEP_TIME);
    // never reached
    return 0;
}

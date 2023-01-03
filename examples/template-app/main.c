#include <stdio.h>
#include "saml21_cpu_debug.h"
#include "saml21_backup_mode.h"

#include "periph_conf.h"
#include "periph/i2c.h"
#include "hdc3020.h"
#include "hdc3020_params.h"

/* use { .pin=EXTWAKE_NONE } to disable */
#define EXTWAKE { \
    .pin=EXTWAKE_PIN6, \
    .polarity=EXTWAKE_HIGH, \
    .flags=EXTWAKE_IN_PU }
#define SLEEP_TIME 5 /* in seconds; -1 to disable */

static saml21_extwake_t extwake = EXTWAKE;
static hdc3020_t hdc3020;

void sensors_init(void)
{
    puts("Sensors init.");
    if (hdc3020_init(&hdc3020, hdc3020_params) == HDC3020_OK) {
        puts("HDC3020 init.");
    }
}

void sensors_read(void)
{
    double temp, hum;

    puts("Sensors read.");
    if (hdc3020_init(&hdc3020, hdc3020_params) == HDC3020_OK) {
        if (hdc3020_read(&hdc3020, &temp, &hum) == HDC3020_OK) {
            printf("Temp: %.1f °C, RH: %.1f %%\n", temp, hum);
        }
        hdc3020_deinit(&hdc3020);
    }
}

void lora_send_command(void)
{
    puts("Lora send command.");
}

void wakeup_task(void)
{
    puts("Wakeup task.");
    sensors_read();
    lora_send_command();
}

void periodic_task(void)
{
    puts("Periodic task.");
    sensors_read();
    lora_send_command();
}

void poweroff_devices(void)
{
    size_t i;

    // turn I2C devices off (leave internal bus I2C_DEV(0) alone)
    for(i = 1; i < I2C_NUMOF; i++) {
        i2c_release(I2C_DEV(i));
        i2c_deinit_pins(I2C_DEV(i));
        gpio_init(i2c_config[i].scl_pin, GPIO_IN_PU);
        gpio_init(i2c_config[i].sda_pin, GPIO_IN_PU);
    }
//  saml21_cpu_debug();
}

int main(void)
{
    switch(saml21_wakeup_cause()) {
        case BACKUP_EXTWAKE:
            wakeup_task();
            break;
        case BACKUP_RTC:
            periodic_task();
            break;
        default:
            if (extwake.pin != EXTWAKE_NONE) {
                printf("GPIO PA%d will wake the board.\n", extwake.pin);
            }
            if (SLEEP_TIME > -1) {
                printf("Periodic task running every %d seconds.\n", SLEEP_TIME);
            }
            sensors_init();
            break;
    }

    puts("Entering backup mode.");
    poweroff_devices();
    saml21_backup_mode_enter(extwake, SLEEP_TIME);
    // never reached
    return 0;
}

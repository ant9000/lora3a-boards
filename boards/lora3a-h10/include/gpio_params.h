#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "Button(SW0)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Enable ACME Sensor 0",
        .pin = GPIO_PIN(PA, 27),
        .mode = GPIO_OUT,
        .flags = ACME0_INIT,
    },
    {
        .name = "Enable ACME Sensor 1",
        .pin = GPIO_PIN(PA, 28),
        .mode = GPIO_OUT,
        .flags = ACME1_INIT,
    },
    {
        .name = "Enable ACME Sensor 2",
        .pin = GPIO_PIN(PA, 31),
        .mode = GPIO_OUT,
        .flags = ACME2_INIT,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */

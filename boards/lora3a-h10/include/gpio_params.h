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
        .name = "Enable I2C 0",
        .pin = GPIO_PIN(PA, 27),
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR,
    },
    {
        .name = "Enable ACME Sensor 1",
        .pin = GPIO_PIN(PA, 28),
        .mode = GPIO_OUT,
#if defined(BME688_ACME1)        
        .flags = SAUL_GPIO_INIT_SET,
#else
        .flags = SAUL_GPIO_INIT_CLEAR,
#endif        
    },
    {
        .name = "Enable ACME Sensor 2",
        .pin = GPIO_PIN(PA, 31),
        .mode = GPIO_OUT,
#if defined(BME688_ACME2)        
        .flags = SAUL_GPIO_INIT_SET,
#else
        .flags = SAUL_GPIO_INIT_CLEAR,
#endif        
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ACME0_INIT
#ifdef ENABLE_ACME0
#define ACME0_INIT 1
#else
#define ACME0_INIT 0
#endif
#endif

#ifndef ACME1_INIT
#ifdef ENABLE_ACME1
#define ACME1_INIT 1
#else
#define ACME1_INIT 0
#endif
#endif

#ifndef ACME2_INIT
#ifdef ENABLE_ACME2
#define ACME2_INIT 1
#else
#define ACME2_INIT 0
#endif
#endif

#define SAUL_GPIO_INIT_VAL(x)    (x?SAUL_GPIO_INIT_SET:SAUL_GPIO_INIT_CLEAR)

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
        .flags = SAUL_GPIO_INIT_VAL(ACME0_INIT),
    },
    {
        .name = "Enable ACME Sensor 1",
        .pin = GPIO_PIN(PA, 28),
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_VAL(ACME1_INIT),
    },
    {
        .name = "Enable ACME Sensor 2",
        .pin = GPIO_PIN(PA, 31),
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_VAL(ACME2_INIT),
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
/** @} */

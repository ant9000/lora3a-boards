#ifndef HDC3020_PARAMS_H
#define HDC3020_PARAMS_H

#include "board.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "hdc3020.h"

#ifndef HDC3020_PARAM_I2C_DEV
#define HDC3020_PARAM_I2C_DEV   I2C_DEV(0)
#endif

#ifndef HDC3020_PARAM_I2C_ADDR
#define HDC3020_PARAM_I2C_ADDR  0x44
#endif

#ifndef HDC3020_ENABLE_PIN
#define HDC3020_ENABLE_PIN      GPIO_UNDEF
#endif

#ifndef HDC3020_ENABLE_ON
#define HDC3020_ENABLE_ON       (1)
#endif

#ifndef HDC3020_START_DELAY
#define HDC3020_START_DELAY     4000
#endif

#ifndef HDC3020_MEAS_DELAY
#define HDC3020_MEAS_DELAY      22000
#endif

#ifndef HDC3020_PARAMS
#define HDC3020_PARAMS                           \
    {                                            \
        .i2c_dev       = HDC3020_PARAM_I2C_DEV,  \
        .i2c_addr      = HDC3020_PARAM_I2C_ADDR, \
        .enable_pin    = HDC3020_ENABLE_PIN,     \
        .enable_on     = HDC3020_ENABLE_ON,      \
        .start_delay   = HDC3020_START_DELAY,    \
        .measure_delay = HDC3020_MEAS_DELAY,     \
    }
#endif


static const hdc3020_params_t hdc3020_params[] =
{
    HDC3020_PARAMS
};

/**
 * @brief   The number of configured sensors
 */
#define HDC3020_NUMOF    ARRAY_SIZE(hdc3020_params)

#endif

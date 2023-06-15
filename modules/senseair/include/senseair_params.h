#ifndef SENSEAIR_PARAMS_H
#define SENSEAIR_PARAMS_H

#include "board.h"
#include "senseair.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SENSEAIR_I2C_DEV
#define SENSEAIR_I2C_DEV    I2C_DEV(0)
#endif

#ifndef SENSEAIR_I2C_ADDR
#define SENSEAIR_I2C_ADDR   0x68
#endif

#ifndef SENSEAIR_ENABLE_PIN
#define SENSEAIR_ENABLE_PIN GPIO_UNDEF
#endif

#ifndef SENSEAIR_PARAMS
#define SENSEAIR_PARAMS { .i2c_dev = SENSEAIR_I2C_DEV,     \
                          .i2c_addr = SENSEAIR_I2C_ADDR,    \
                          .enable_pin = SENSEAIR_ENABLE_PIN }
#endif

#ifndef SENSEAIR_SAULINFO
#define SENSEAIR_SAULINFO           { .name = "senseair" }
#endif

static const senseair_params_t senseair_params[] = {
    SENSEAIR_PARAMS
};

static const saul_reg_info_t senseair_saul_info[] =
{
    SENSEAIR_SAULINFO
};

#ifdef __cplusplus
}
#endif

#endif /* SENSEAIR_H */

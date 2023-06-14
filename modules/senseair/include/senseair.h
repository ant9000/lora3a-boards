#ifndef SENSEAIR_H
#define SENSEAIR_H

#include "periph/i2c.h"
#include "periph/gpio.h"

typedef struct {
    i2c_t i2c_dev;                     /**< I2C device which is used */
    uint8_t i2c_addr;                  /**< I2C address */
    gpio_t enable_pin;
} senseair_params_t;

enum {
    SENSEAIR_OK              =  0,     /**< everything was fine */
    SENSEAIR_ERR_NODEV       = -1,     /**< did not detect device */
    SENSEAIR_ERR_MEAS        = -2,     /**< could not start measure */
    SENSEAIR_ERR_ABC_INVALID = -3,     /**< invalid ABC calibration data */
    SENSEAIR_ERR_ABC_WRITE   = -4,     /**< could not write ABC calibration */
    SENSEAIR_ERR_ABC_READ    = -5,     /**< could not read ABC calibration */
};

typedef struct {
    char signature[5];
    uint32_t last_update;
    uint8_t  data[32];
} senseair_abc_data_t;

typedef struct {
    senseair_params_t params;  /**< Device Parameters */
} senseair_dev_t;

int senseair_init(senseair_dev_t* dev, const senseair_params_t* params);
int senseair_read(const senseair_dev_t *dev, uint16_t *conc_ppm, uint16_t *temp_cC);
int senseair_read_abc_data(const senseair_dev_t *dev, senseair_abc_data_t *abc_data);
int senseair_write_abc_data(const senseair_dev_t *dev, senseair_abc_data_t *abc_data);

#endif /* SENSEAIR_H */

#ifndef HDC_H

#include "periph/i2c.h"
#include "periph/gpio.h"

typedef struct {
    i2c_t i2c_dev;                      /**< I2C device which is used */
    uint8_t i2c_addr;                   /**< I2C address */
    gpio_t enable_pin;
    uint32_t start_delay;
    uint32_t measure_delay;
} hdc3020_params_t;

typedef struct {
    hdc3020_params_t params;  /**< Device Parameters */
} hdc3020_t;

enum {
    HDC3020_OK        =  0,     /**< everything was fine */
    HDC3020_ERR_BUS   = -1,     /**< bus error */
    HDC3020_ERR_NODEV = -2,     /**< did not detect device */
    HDC3020_ERR_MEAS  = -3,     /**< could not start measure */
};

int hdc3020_init(hdc3020_t* dev, const hdc3020_params_t* params);
int hdc3020_read(const hdc3020_t *dev, double *temp, double *hum);
void hdc3020_deinit(hdc3020_t* dev);

#endif /* HDC_H */

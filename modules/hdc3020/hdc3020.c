#include <stdio.h>
#include <inttypes.h>

#include "hdc3020.h"
#include "ztimer.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

int hdc3020_init(hdc3020_t* dev, const hdc3020_params_t* params)
{
    assert(dev && params);
    dev->params = *params;

    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_init(dev->params.enable_pin, GPIO_OUT);
        gpio_write(dev->params.enable_pin, dev->params.enable_on);
        ztimer_sleep(ZTIMER_USEC, dev->params.start_delay);
    }
    // TODO: check for sensor

    return HDC3020_OK;
}

void hdc3020_deinit(const hdc3020_t* dev)
{
    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_write(dev->params.enable_pin, 1 - dev->params.enable_on);
    }
}

int hdc3020_read(const hdc3020_t* dev, double *temp, double *hum)
{
    int status = 0, retry = 10;
    uint8_t command[2] = {0x24, 0x00};
    uint8_t data[6];

    i2c_acquire(dev->params.i2c_dev);
    if (i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, command, sizeof(command), 0)) {
        return HDC3020_ERR_MEAS;
    }
    ztimer_sleep(ZTIMER_USEC, dev->params.measure_delay);
    do {
        status = i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, data, sizeof(data), 0);
        if (status) {
            retry--;
            if (retry < 0) {
                return HDC3020_ERR_MEAS;
            }
            ztimer_sleep(ZTIMER_USEC, 100);
        }
    } while(status);

    i2c_release(dev->params.i2c_dev);

    if (temp) {
        *temp = ((data[0] << 8) + data[1]) * 175. / (1 << 16) - 45;
    }
    if (hum) {
        *hum =  ((data[3] << 8) + data[4]) * 100. / (1 << 16);
    }
    return HDC3020_OK;
}

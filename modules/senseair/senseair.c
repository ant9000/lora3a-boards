#include <string.h>

#include "senseair.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/rtc.h"
#include "rtc_utils.h"
#include "ztimer.h"
#include "od.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SENSEAIR_ERROR_STATUS_REG     0x00
#define SENSEAIR_CONCENTRATION_REG    0x06
#define SENSEAIR_TEMPERATURE_REG      0x08
#define SENSEAIR_MEASURE_COUNT_REG    0x0d
#define SENSEAIR_START_MEASURE_REG    0x93
#define SENSEAIR_MEASURE_MODE_REG     0x95
#define SENSEAIR_SAVED_STATE_REG      0xc0

#define BSWAP(data)     (((data & 0xff) << 8) + ((data >> 8) & 0xff))

static const char *senseair_signature="SUNR";

void _print_senseair_data(const senseair_abc_data_t *abc_data)
{
    (void)abc_data;
#if ENABLE_DEBUG
    printf("Signature: %s; last update: %lu\n", abc_data->signature, abc_data->last_update);
    od_hex_dump(abc_data->data, sizeof(abc_data->data), 0);
#endif
}

void _print_time(struct tm *time)
{
    (void)time;
#if ENABLE_DEBUG
    printf("%04i-%02i-%02i %02i:%02i:%02i\n",
        time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
        time->tm_hour, time->tm_min, time->tm_sec
    );
#endif
}

int senseair_init(senseair_t* dev, const senseair_params_t* params)
{
    int res;
    uint8_t reg;
    uint16_t data = 0;

    assert(dev && params);
    dev->params = *params;

    DEBUG("Activating Senseair sensor.\n");
    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_init(dev->params.enable_pin, GPIO_OUT);
        gpio_set(dev->params.enable_pin);
        ztimer_sleep(ZTIMER_MSEC, 20);
    }

    res = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_ERROR_STATUS_REG, &data, 2, 0);
    if (res) {
        DEBUG("ERROR %d reading I2C.\n", res);
        gpio_clear(dev->params.enable_pin);
        return SENSEAIR_ERR_NODEV;
    }
    DEBUG("Status: 0x%04x\n", data);
    if (data == 0x8000) {
        DEBUG("Senseair: active.\n");
    } else {
        DEBUG("Sensair: not found.\n");
        gpio_clear(dev->params.enable_pin);
        return SENSEAIR_ERR_NODEV;
    }
    i2c_read_reg(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_MEASURE_MODE_REG, &reg, 0);
    if (reg == 1) {
        DEBUG("Single measurement mode: active.\n");
    } else {
        i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_MEASURE_MODE_REG, 1, 0);
        DEBUG("Single measurement mode: set.\n");
    }
    return SENSEAIR_OK;
}

int senseair_read(const senseair_t *dev, uint16_t *conc_ppm, int16_t *temp_cC)
{
    int res, count = 0;
    uint8_t reg;
    uint16_t data = 0;

    DEBUG("Starting measure.\n");
    res = i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_START_MEASURE_REG, 1, 0);
    if (res) {
        DEBUG("ERROR %d starting measure.\n", res);
        return SENSEAIR_ERR_MEAS;
    }
    reg = 0;
    while (reg == 0) {
        ztimer_sleep(ZTIMER_MSEC, 50);
        res = i2c_read_reg(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_MEASURE_COUNT_REG, &reg, 0);
        if (res) {
            DEBUG("ERROR %d during measure.\n", res);
            return SENSEAIR_ERR_MEAS;
        }
        count++;
        if (count > 60) {
            DEBUG("ERROR: measure taking too long.\n");
            return SENSEAIR_ERR_MEAS;
        }
    }
    if (conc_ppm != NULL) {
        res = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_CONCENTRATION_REG, &data, 2, 0);
        if (res) {
            DEBUG("ERROR %d reading concentration.\n", res);
            return SENSEAIR_ERR_MEAS;
        }
        *conc_ppm = BSWAP(data);
        DEBUG("Concentration: %d ppm\n", *conc_ppm);
    }
    if (temp_cC != NULL) {
        res = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_TEMPERATURE_REG, &data, 2, 0);
        if (res) {
            DEBUG("ERROR %d reading temperature.\n", res);
            return SENSEAIR_ERR_MEAS;
        }
        *temp_cC = (int16_t)BSWAP(data);
        DEBUG("Temperature: %4.2f °C\n", (*temp_cC/100.));
    }
    return SENSEAIR_OK;
}

int senseair_write_abc_data(const senseair_t *dev, senseair_abc_data_t *abc_data)
{
    uint16_t data = 0;
    struct tm time;
    uint32_t tstamp;

    if (strncmp(abc_data->signature, senseair_signature, sizeof(abc_data->signature))!=0) {
        DEBUG("ABC calibration data is not valid.\n");
        return SENSEAIR_ERR_ABC_INVALID;
    }
    DEBUG("Writing ABC calibration data to sensor.\n");
    _print_senseair_data(abc_data);
    // bytes 0x04, 0x05 in senseair_data.data represents ABC time (in hours)
    // it must be incremented every hour
    rtc_get_time(&time);
    _print_time(&time);
    tstamp=rtc_mktime(&time);
    DEBUG("tstamp=%lu\n", tstamp);
    if (tstamp - abc_data->last_update > 3600) {
        DEBUG("ABC time update: 0x%02x%02x -> ", abc_data->data[4], abc_data->data[5]);
        data = (abc_data->data[4] << 8) + abc_data->data[5] + (tstamp - abc_data->last_update) / 3600;
        abc_data->data[4] = (data >> 8) & 0xff;
        abc_data->data[5] = data & 0xff;
        DEBUG("0x%02x%02x\n", abc_data->data[4], abc_data->data[5]);
        abc_data->last_update = tstamp;
    }
    if (i2c_write_regs(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_SAVED_STATE_REG, abc_data->data, sizeof(abc_data->data), 0)) {
        DEBUG("ERROR: writing ABC calibration data failed.\n");
        return SENSEAIR_ERR_ABC_WRITE;
    }
    return SENSEAIR_OK;
}

int senseair_read_abc_data(const senseair_t *dev, senseair_abc_data_t *abc_data)
{
    struct tm time;

    DEBUG("Reading ABC calibration data from sensor.\n");
    strncpy(abc_data->signature, senseair_signature, sizeof(abc_data->signature));
    if (abc_data->last_update == 0) {
        rtc_get_time(&time);
        _print_time(&time);
        abc_data->last_update=rtc_mktime(&time);
    }
    memset(abc_data->data, 0, sizeof(abc_data->data));
    if (i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, SENSEAIR_SAVED_STATE_REG, abc_data->data, sizeof(abc_data->data), 0)) {
        DEBUG("ERROR: reading ABC calibration data failed.\n");
        return SENSEAIR_ERR_ABC_READ;
    }
    _print_senseair_data(abc_data);
    return SENSEAIR_OK;
}

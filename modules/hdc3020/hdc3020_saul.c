#include "saul.h"
#include "hdc3020.h"

static int read_temperature(const void *dev, phydat_t *res)
{
    double temp;
    if (hdc3020_read(dev, &temp, NULL) != HDC3020_OK) {
        return -ECANCELED;
    }

    res->val[0] = (int16_t)(temp*100);
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

static int read_humidity(const void *dev, phydat_t *res)
{
    double hum;
    if (hdc3020_read(dev, NULL, &hum) != HDC3020_OK) {
        return -ECANCELED;
    }
    res->val[0] = (int16_t)(hum*100);
    res->unit = UNIT_PERCENT;
    res->scale = -2;

    return 1;
}

const saul_driver_t hdc3020_saul_temp_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

const saul_driver_t hdc3020_saul_hum_driver = {
    .read = read_humidity,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_HUM,
};

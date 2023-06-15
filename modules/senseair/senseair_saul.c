#include "saul.h"
#include "senseair.h"

static int read_co2(const void *dev, phydat_t *res)
{
    if (senseair_read(dev, (uint16_t *)&res->val[0], NULL) != SENSEAIR_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_PPM;
    res->scale = 0;
    return 1;
}

static int read_temperature(const void *dev, phydat_t *res)
{
    if (senseair_read(dev, NULL, &res->val[0]) != SENSEAIR_OK) {
        return -ECANCELED;
    }
    res->unit = UNIT_TEMP_C;
    res->scale = -2;

    return 1;
}

const saul_driver_t senseair_saul_driver = {
    .read = read_co2,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_CO2,
};

const saul_driver_t senseair_saul_temp_driver = {
    .read = read_temperature,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_TEMP,
};

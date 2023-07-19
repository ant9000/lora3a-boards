#include "saul.h"
#include "h10_adc.h"

static int read_vcc(const void *dev, phydat_t *res)
{
    int32_t value;
    value = h10_adc_read_vcc(dev);

    res->unit = UNIT_V;
    res->scale = -3;
    phydat_fit(res, &value, 1);

    return 1;
}

static int read_vpanel(const void *dev, phydat_t *res)
{
    int32_t value;
    value = h10_adc_read_vpanel(dev);

    res->unit = UNIT_V;
    res->scale = -3;
    phydat_fit(res, &value, 1);

    return 1;
}

const saul_driver_t h10_adc_saul_vcc_driver = {
    .read = read_vcc,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_VOLTAGE,
};

const saul_driver_t h10_adc_saul_vpanel_driver = {
    .read = read_vpanel,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_VOLTAGE,
};

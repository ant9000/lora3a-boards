#include <inttypes.h>

#include "h10_adc.h"
#include "ztimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

int h10_adc_init(h10_adc_t* dev, const h10_adc_params_t* params)
{
    assert(dev && params);
    dev->params = *params;

    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_init(dev->params.enable_pin, GPIO_OUT);
        gpio_write(dev->params.enable_pin, dev->params.enable_on);
        ztimer_sleep(ZTIMER_USEC, dev->params.start_delay);
    }

    return H10_ADC_OK;
}

void h10_adc_deinit(const h10_adc_t* dev)
{
    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_write(dev->params.enable_pin, 1 - dev->params.enable_on);
    }
}

int32_t h10_adc_read_vcc(const h10_adc_t* dev)
{
    (void)dev;
    int32_t val = adc_sample(0, ADC_RES_16BIT);
    // ADC line 0 reads Vcc/4, against VREFINT of 1V
    // we want mV
    int32_t vcc = (val * 4 * 1000) >> 16;
    return vcc;
}

int32_t h10_adc_read_vpanel(const h10_adc_t* dev)
{
    (void)dev;
    int32_t val = adc_sample(1, ADC_RES_16BIT);
    // ADC line 1 reads Vpanel against VREFINT of 1V, with a resistor partition of 75k over 220k
    // we want mV
    int32_t vpanel = (val * (220 + 75) / 75 * 1000) >> 16;
    return vpanel;
}

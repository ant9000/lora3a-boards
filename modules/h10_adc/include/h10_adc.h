#ifndef H10ADC_H
#define H10ADC_H

#include "saul.h"
#include "periph/gpio.h"

typedef struct {
    gpio_t enable_pin;
    uint8_t enable_on;
    uint32_t start_delay;
} h10_adc_params_t;

typedef struct {
    h10_adc_params_t params;  /**< Device Parameters */
} h10_adc_t;

extern const saul_driver_t h10_adc_saul_vcc_driver;
extern const saul_driver_t h10_adc_saul_vpanel_driver;

enum {
    H10_ADC_OK = 0,     /**< everything was fine */
};

int h10_adc_init(h10_adc_t* dev, const h10_adc_params_t* params);
int32_t h10_adc_read_vcc(const h10_adc_t* dev);
int32_t h10_adc_read_vpanel(const h10_adc_t* dev);
void h10_adc_deinit(const h10_adc_t* dev);

#endif /* H10ADC_H */

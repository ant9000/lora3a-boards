#ifndef H10ADC_PARAMS_H
#define H10ADC_PARAMS_H

#include "board.h"
#include "h10_adc.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef H10ADC_ENABLE_PIN
#define H10ADC_ENABLE_PIN      GPIO_PIN(PA, 27)
#endif

#ifndef H10ADC_ENABLE_ON
#define H10ADC_ENABLE_ON       (1)
#endif

#ifndef H10ADC_START_DELAY
#define H10ADC_START_DELAY     10000
#endif

static const h10_adc_params_t h10_adc_params[] =
{
    {
        .enable_pin    = H10ADC_ENABLE_PIN,
        .enable_on     = H10ADC_ENABLE_ON,
        .start_delay   = H10ADC_START_DELAY,
    },
};

static const saul_reg_info_t h10_adc_saul_info[] =
{
    { .name = "Vcc" },
    { .name = "Vpanel" },
};

#ifdef __cplusplus
}
#endif

#endif

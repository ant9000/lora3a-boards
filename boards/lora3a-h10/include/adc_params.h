#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "Vcc",
        .line = ADC_LINE(0),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "Vpanel",
        .line = ADC_LINE(1),
        .res  = ADC_RES_12BIT,
    },
    {
        .name = "Bandgap",
        .line = ADC_LINE(2),
        .res  = ADC_RES_12BIT,
    }
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */


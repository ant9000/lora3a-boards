#include "log.h"
#include "assert.h"
#include "saul_reg.h"
#include "h10_adc.h"
#include "h10_adc_params.h"

static h10_adc_t h10_adc_dev;
static saul_reg_t saul_entries[2];

void auto_init_h10_adc(void)
{
    int res;

    LOG_DEBUG("[auto_init_saul] initializing h10_adc\n");
    res = h10_adc_init(&h10_adc_dev, &h10_adc_params[0]);
    if (res < 0) {
        LOG_ERROR("[auto_init_saul] error initializing h10_adc\n");
        return;
    }

    saul_entries[0].dev = &h10_adc_dev;
    saul_entries[0].name = h10_adc_saul_info[0].name;
    saul_entries[0].driver = &h10_adc_saul_vcc_driver;
    saul_reg_add(&saul_entries[0]);

    saul_entries[1].dev = &h10_adc_dev;
    saul_entries[1].name = h10_adc_saul_info[1].name;
    saul_entries[1].driver = &h10_adc_saul_vpanel_driver;
    saul_reg_add(&saul_entries[1]);
}

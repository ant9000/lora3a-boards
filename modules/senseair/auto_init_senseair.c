#include "log.h"
#include "assert.h"
#include "saul_reg.h"
#include "senseair.h"
#include "senseair_params.h"

#define SENSEAIR_NUM    ARRAY_SIZE(senseair_params)

#define SENSEAIR_SAULINFO_NUM   (sizeof(senseair_saul_info) / \
                                 sizeof(senseair_saul_info[0]))

static senseair_t senseair_devs[SENSEAIR_NUM];
static saul_reg_t saul_entries[SENSEAIR_NUM * 2];

void auto_init_senseair(void)
{
    assert(SENSEAIR_NUM == SENSEAIR_SAULINFO_NUM);

    for (unsigned int i = 0; i < SENSEAIR_NUM; i++) {
        int res;

        LOG_DEBUG("[auto_init_saul] initializing senseair #%u\n", i);

        res = senseair_init(&senseair_devs[i], &senseair_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing senseair #%u\n", i);
            continue;
        }

        saul_entries[2 * i].dev = &senseair_devs[i];
        saul_entries[2 * i].name = senseair_saul_info[i].name;
        saul_entries[2 * i].driver = &senseair_saul_driver;
        saul_reg_add(&saul_entries[2 * i]);

        saul_entries[2 * i + 1].dev = &senseair_devs[i];
        saul_entries[2 * i + 1].name = senseair_saul_info[i].name;
        saul_entries[2 * i + 1].driver = &senseair_saul_temp_driver;
        saul_reg_add(&saul_entries[2 * i + 1]);
    }
}


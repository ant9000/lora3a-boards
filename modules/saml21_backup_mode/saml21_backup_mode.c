#include <stdio.h>
#include "board.h"
#include "saml21_backup_mode.h"

#include "periph/pm.h"
#include "periph/rtc.h"
#include "rtc_utils.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

saml21_wakeup_cause_t saml21_wakeup_cause(void)
{
    saml21_wakeup_cause_t cause;

    DEBUG("RSTC->RCAUSE:   %02x\n", RSTC->RCAUSE.reg);
    DEBUG("RSTC->BKUPEXIT: %02x\n", RSTC->BKUPEXIT.reg);
    DEBUG("RSTC->WKCAUSE:  %02x\n", RSTC->WKCAUSE.reg & 0xff);
    switch (RSTC->RCAUSE.reg) {
    case RSTC_RCAUSE_POR:
        cause = POWER_ON;
        DEBUG("Cause: POWER_ON\n");
        break;
    case RSTC_RCAUSE_BOD12:
    case RSTC_RCAUSE_BOD33:
        cause = BROWN_OUT;
        DEBUG("Cause: BROWN_OUT\n");
        break;
    case RSTC_RCAUSE_EXT:
        cause = EXTERNAL;
        DEBUG("Cause: EXTERNAL\n");
        break;
    case RSTC_RCAUSE_WDT:
        cause = WATCHDOG;
        DEBUG("Cause: WATCHDOG\n");
        break;
    case RSTC_RCAUSE_SYST:
        cause = SYSTEM;
        DEBUG("Cause: SYSTEM\n");
        break;
    case RSTC_RCAUSE_BACKUP:
        switch (RSTC->BKUPEXIT.reg) {
        case RSTC_BKUPEXIT_EXTWAKE:
            cause = BACKUP_EXTWAKE;
            DEBUG("Cause: BACKUP_EXTWAKE\n");
            break;
        case RSTC_BKUPEXIT_RTC:
            cause = BACKUP_RTC;
            DEBUG("Cause: BACKUP_RTC\n");
            break;
        case RSTC_BKUPEXIT_BBPS:
            cause = BACKUP_POWER_SWITCH;
            DEBUG("Cause: BACKUP_POWER_SWITCH\n");
            break;
        default:
            cause = BACKUP_EXTWAKE;
            DEBUG("Cause: fallback to BACKUP_EXTWAKE\n");
            break;
        }
        break;
    default:
        cause = POWER_ON;
        DEBUG("Cause: fallback to POWER_ON\n");
        break;
    }
    return cause;
}

uint8_t saml21_wakeup_pins(void)
{
    uint8_t pins = RSTC->WKCAUSE.reg & 0xff;
#if ENABLE_DEBUG
    printf("Wakeup pins: ");
    for (int i = 0; i < 8; i++) {
        if (pins & (1 << i)) {
            printf(" PA%02d", i);
        }
    }
    printf("\n");
#endif
    return pins;
}

void waitCurrentMeasureBM(uint32_t milliseconds, char* step) {
	printf("waitCurrentMeasure %s\n", step);
	ztimer_sleep(ZTIMER_MSEC, milliseconds);
}

void saml21_backup_mode_enter(uint8_t RadioOffRequested, saml21_extwake_t *extwake, int sleep_secs,
                              uint8_t resetTime)
{
    uint32_t seconds;
    uint8_t emergency_sleep = 0;

    if (RadioOffRequested) {
    #ifdef MODULE_SX127X
        // turn radio off
        sx127x_t sx127x;
        sx127x.params = sx127x_params[0];
        spi_init(sx127x.params.spi);
    #ifdef TCXO_PWR_PIN
        gpio_set(TCXO_PWR_PIN);
    #endif
        sx127x_init(&sx127x);
        sx127x_set_sleep(&sx127x);
    #ifdef TCXO_PWR_PIN
        gpio_clear(TCXO_PWR_PIN);
    #endif
    #ifdef TX_OUTPUT_SEL_PIN
        gpio_clear(TX_OUTPUT_SEL_PIN);
    #endif
        spi_deinit_pins(sx127x.params.spi);
    #endif
    }

    if (extwake != NULL) {
        int pin_count = 0;
        for (int i=0; i<8; i++) {
            saml21_extwake_t *e = &extwake[i];
            if (e->pin == EXTWAKE_NONE) {
                break;
            }
            pin_count++;
            DEBUG("extwake[%d].pin=%d; extwake[%d].polarity=%d\n", i, e->pin, i, e->polarity);
            gpio_init(GPIO_PIN(PA, e->pin), (gpio_mode_t)e->flags);

            // wait for pin to settle
            uint8_t retry = 5;
            while (retry > 0) {
                retry--;
                if (((PORT->Group[0].IN.reg >> e->pin) & 1) == e->polarity) {
                    break;
                }
                ztimer_sleep(ZTIMER_MSEC, 10);
            }
            DEBUG("RETRY EXTWAKE: %d.\n", retry);
            if (retry == 0) {
                printf("EXTWAKE PIN STUCK %s.\n",
                       ((PORT->Group[0].IN.reg >> e->pin) & 1) ? "HIGH": "LOW");
                emergency_sleep++;
            }
            else {
                RSTC->WKEN.reg |= 1 << e->pin;
                if (e->polarity == EXTWAKE_LOW) {
                    RSTC->WKPOL.reg |= (1 << e->pin);
                }
                else {
                    RSTC->WKPOL.reg &= ~(1 << e->pin);
                }
            }
        }
        if (pin_count && emergency_sleep == pin_count) {
            DEBUG("All pins are stuck: disabling wakeup on GPIO.\n");
            RSTC->WKEN.reg = 0;
        }
    }
    else {
        RSTC->WKEN.reg = 0;
    }

    if (emergency_sleep && sleep_secs <= 0) {
        sleep_secs = EMERGENCY_SLEEP_TIME;
    }

    if (sleep_secs > 0) {
        seconds = sleep_secs;

        struct tm t;
        if (resetTime) {
            rtc_localtime(0, &t);
            rtc_set_time(&t);
        }
        else {
            rtc_get_time(&t);
        }
        uint32_t alarm = rtc_mktime(&t) + seconds;
        rtc_localtime(alarm, &t);
        rtc_set_alarm(&t, NULL, NULL);
    }

    pm_set(SAML21_PM_MODE_BACKUP);
}

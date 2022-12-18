#include <stdio.h>
#include "board.h"
#include "saml21_backup_mode.h"

#include "periph/gpio.h"
#include "periph/pm.h"
#include "periph/rtt.h"

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
    DEBUG("Pins: 0x%02x\n", pins);
    return pins;
}

void saml21_backup_mode_enter(int wakeup_pin, int sleep_secs)
{
    uint8_t extwake;
    uint32_t seconds;

    if (wakeup_pin >= 0 && wakeup_pin <= 7) {
        extwake = wakeup_pin & 0xff;
        gpio_init(GPIO_PIN(PA, extwake), GPIO_IN_PU);
        // wait for pin to settle
        while (!(PORT->Group[0].IN.reg & (1 << extwake))) {}
        RSTC->WKEN.reg = 1 << extwake;
        RSTC->WKPOL.reg &= ~(1 << extwake);
    }
    if (sleep_secs > 0) {
        seconds = sleep_secs;
        rtt_set_counter(0);
        rtt_set_alarm(RTT_SEC_TO_TICKS(seconds), NULL, NULL);
    }
    pm_set(SAML21_PM_MODE_BACKUP);
}

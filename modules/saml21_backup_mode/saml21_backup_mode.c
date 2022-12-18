#include "board.h"
#include "saml21_backup_mode.h"

#include "periph/gpio.h"
#include "periph/pm.h"
#include "periph/rtt.h"

saml21_wakeup_cause_t saml21_wakeup_cause(void)
{
    saml21_wakeup_cause_t cause;

    switch (RSTC->RCAUSE.reg | RSTC_RCAUSE_MASK) {
    case RSTC_RCAUSE_POR:
        cause = POWER_ON;
        break;
    case RSTC_RCAUSE_BOD12:
    case RSTC_RCAUSE_BOD33:
        cause = BROWN_OUT;
        break;
    case RSTC_RCAUSE_EXT:
        cause = EXTERNAL;
        break;
    case RSTC_RCAUSE_WDT:
        cause = WATCHDOG;
        break;
    case RSTC_RCAUSE_SYST:
        cause = SYSTEM;
        break;
    case RSTC_RCAUSE_BACKUP:
        switch (RSTC->BKUPEXIT.reg | RSTC_BKUPEXIT_MASK) {
        case RSTC_BKUPEXIT_EXTWAKE:
            cause = BACKUP_EXTWAKE;
            break;
        case RSTC_BKUPEXIT_RTC:
            cause = BACKUP_RTC;
            break;
        case RSTC_BKUPEXIT_BBPS:
            cause = BACKUP_POWER_SWITCH;
            break;
        }
        break;
    default:
        cause = RSTC_RCAUSE_POR;
        break;
    }
    return cause;
}

uint8_t saml21_wakeup_pins(void)
{
    return RSTC->WKCAUSE.reg && 0xff;
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

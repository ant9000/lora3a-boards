#include <stdio.h>
#include "board.h"
#include "saml21_backup_mode.h"

#include "periph/pm.h"
#include "periph/rtt.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#include "sx127x_internal.h"
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
    for(int i=0; i<8; i++) {
        if (pins & (1 << i)) { printf(" PA%02d", i); }
    }
    printf("\n");
#endif
    return pins;
}

void saml21_backup_mode_enter(saml21_extwake_t extwake, int sleep_secs)
{
    uint32_t seconds;

#ifdef MODULE_SX127X
    // turn radio off
    sx127x_t sx127x;
    sx127x.params = sx127x_params[0];
    spi_release(sx127x.params.spi);
    spi_deinit_pins(sx127x.params.spi);
    if (gpio_is_valid(sx127x.params.reset_pin)) {
        gpio_init(sx127x.params.reset_pin, GPIO_OUT);
        gpio_write(sx127x.params.reset_pin, SX127X_POR_ACTIVE_LOGIC_LEVEL);
    }
    if (gpio_is_valid(sx127x.params.dio0_pin)) {
       gpio_irq_disable(sx127x.params.dio0_pin);
       gpio_init(sx127x.params.dio0_pin, SX127X_DIO_PULL_MODE);
    }
    if (gpio_is_valid(sx127x.params.dio1_pin)) {
       gpio_irq_disable(sx127x.params.dio1_pin);
       gpio_init(sx127x.params.dio1_pin, SX127X_DIO_PULL_MODE);
    }
    if (gpio_is_valid(sx127x.params.dio2_pin)) {
       gpio_irq_disable(sx127x.params.dio2_pin);
       gpio_init(sx127x.params.dio2_pin, SX127X_DIO_PULL_MODE);
    }
    if (gpio_is_valid(sx127x.params.dio3_pin)) {
       gpio_irq_disable(sx127x.params.dio3_pin);
       gpio_init(sx127x.params.dio3_pin, SX127X_DIO_PULL_MODE);
    }
#ifdef TCXO_PWR_PIN
    if (gpio_is_valid(TCXO_PWR_PIN)) gpio_clear(TCXO_PWR_PIN);
#endif
#ifdef TX_OUTPUT_SEL_PIN
    if (gpio_is_valid(TX_OUTPUT_SEL_PIN)) gpio_clear(TX_OUTPUT_SEL_PIN);
#endif
#endif

    if (extwake.pin != EXTWAKE_NONE) {
        gpio_init(GPIO_PIN(PA, extwake.pin), extwake.flags);
        // wait for pin to settle
        while (((PORT->Group[0].IN.reg >> extwake.pin) & 1) != extwake.polarity) {}
        RSTC->WKEN.reg = 1 << extwake.pin;
        if (extwake.polarity == EXTWAKE_LOW) {
            RSTC->WKPOL.reg |= (1 << extwake.pin);
        } else {
            RSTC->WKPOL.reg &= ~(1 << extwake.pin);
        }
    } else {
        RSTC->WKEN.reg = 0;
    }
    if (sleep_secs > 0) {
        seconds = sleep_secs;
        rtt_set_counter(0);
        rtt_set_alarm(RTT_SEC_TO_TICKS(seconds), NULL, NULL);
    }
    pm_set(SAML21_PM_MODE_BACKUP);
}

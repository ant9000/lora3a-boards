#ifndef SAML21_BACKUP_MODE
#define SAML21_BACKUP_MODE

#include <inttypes.h>

typedef enum {
    POWER_ON,
    BROWN_OUT,
    EXTERNAL,
    WATCHDOG,
    SYSTEM,
    BACKUP_EXTWAKE,
    BACKUP_RTC,
    BACKUP_POWER_SWITCH,
} saml21_wakeup_cause_t;

saml21_wakeup_cause_t saml21_wakeup_cause(void);
uint8_t saml21_wakeup_pins(void);
void saml21_backup_mode_enter(int wakeup_pin, int sleep_secs);

#endif

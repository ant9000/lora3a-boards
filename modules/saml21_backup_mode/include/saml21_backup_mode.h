#ifndef SAML21_BACKUP_MODE
#define SAML21_BACKUP_MODE

#include <inttypes.h>
#include "periph/gpio.h"

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

typedef enum {
    EXTWAKE_NONE = -1,
    EXTWAKE_PIN0,
    EXTWAKE_PIN1,
    EXTWAKE_PIN2,
    EXTWAKE_PIN3,
    EXTWAKE_PIN4,
    EXTWAKE_PIN5,
    EXTWAKE_PIN6,
    EXTWAKE_PIN7,
} saml21_extwake_pin_t;

typedef enum {
    EXTWAKE_LOW,
    EXTWAKE_HIGH,
} saml21_extwake_pin_polarity_t;

typedef enum {
    EXTWAKE_IN = GPIO_IN,
    EXTWAKE_IN_PD = GPIO_IN_PD,
    EXTWAKE_IN_PU = GPIO_IN_PU,
} saml21_extwake_pin_flags;

typedef struct {
    saml21_extwake_pin_t pin;
    saml21_extwake_pin_polarity_t polarity;
    saml21_extwake_pin_flags flags;
} saml21_extwake_t;

saml21_wakeup_cause_t saml21_wakeup_cause(void);
uint8_t saml21_wakeup_pins(void);
void saml21_backup_mode_enter(uint8_t RadioOffRequested, saml21_extwake_t *extwake, int sleep_secs,
                              uint8_t resetTime);

#define RADIO_OFF_NOT_REQUESTED 0
#define RADIO_OFF_REQUESTED 1
#endif

/**
 * @name    Emergency Sleep for Wake Pin Stuck
 * @{
 */
#ifndef EMERGENCY_SLEEP_TIME
#define EMERGENCY_SLEEP_TIME 30
#endif
/** @} */

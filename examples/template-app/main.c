#include <stdio.h>
#include "saml21_backup_mode.h"

#define WAKEUP_PIN 6  /* from 0 = PA00 to 7 = PA07; -1 to disable; BTN_0 is 6 */
#define SLEEP_TIME 5 /* in seconds; -1 to disable */

void sensors_init(void)
{
  puts("Sensors init.");
}

void sensors_read(void)
{
  puts("Sensors read.");
}

void lora_send_command(void)
{
  puts("Lora send command.");
}

void periodic_task(void)
{
  puts("Periodic task.");
}

int main(void)
{
    switch(saml21_wakeup_cause()) {
        case BACKUP_EXTWAKE:
            sensors_read();
            lora_send_command();
            break;
        case BACKUP_RTC:
            periodic_task();
            break;
        default:
            if (WAKEUP_PIN > -1) {
                printf("GPIO PA%d will wake the board.\n", WAKEUP_PIN);
            }
            if (SLEEP_TIME > -1) {
                printf("Periodic task running every %d seconds.\n", SLEEP_TIME);
            }
            sensors_init();
            break;
    }

    puts("Entering backup mode.");
    saml21_backup_mode_enter(WAKEUP_PIN, SLEEP_TIME);
    // never reached
    return 0;
}

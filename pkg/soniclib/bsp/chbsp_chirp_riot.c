#include <stdio.h>
#include "ztimer.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#include "soniclib.h"
#include "chirp_bsp.h"

#define __unused__ (void)

// TODO: temporarily inline config data
#define CHBSP_MAX_DEVICES 1
#define CHBSP_NUM_BUSES 1
#define CHBSP_RTC_CAL_PULSE_MS (100)

#define LORA3A_SENSOR2_POWER    GPIO_PIN(PA, 31)

#define CHIP_PROG_0             GPIO_PIN(PA, 7)
#define CHIP_I2C_0              I2C_DEV(2)
// END

static void find_sensors(void) {
    uint8_t sig_bytes[2];

    /* Deassert PROG on all sensor ports */
    gpio_init(CHIP_PROG_0, GPIO_OUT);
    gpio_set(CHIP_PROG_0);

    /* Power on sensor busses */
    gpio_set(LORA3A_SENSOR2_POWER);
    chbsp_delay_us(5000);

    /* check sensor 0 */
    gpio_clear(CHIP_PROG_0);
    i2c_acquire(CHIP_I2C_0);
    sig_bytes[0] = 0;
    sig_bytes[1] = 0;
    i2c_read_regs(CHIP_I2C_0, CH_I2C_ADDR_PROG, 0x00, sig_bytes, 2, 0);
    printf("Chirp sensor 0 ");
    if ((sig_bytes[0] == CH_SIG_BYTE_0) && (sig_bytes[1] == CH_SIG_BYTE_1)) {
        printf("found\n");
    } else {
        printf("not found\n");
    }
    gpio_set(CHIP_PROG_0);
}

void chbsp_board_init(ch_group_t *grp_ptr) {
    /* Initialize group descriptor */
    grp_ptr->num_ports = CHBSP_MAX_DEVICES;
    grp_ptr->num_buses = CHBSP_NUM_BUSES;
    grp_ptr->rtc_cal_pulse_ms = CHBSP_RTC_CAL_PULSE_MS;

    /* Probe I2C bus to find connected sensor(s) */
    find_sensors();
}

void chbsp_delay_us(uint32_t us) {
  ztimer_sleep(ZTIMER_USEC, us);
}
void chbsp_delay_ms(uint32_t ms) {
  ztimer_sleep(ZTIMER_MSEC, ms);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void chbsp_reset_assert(void) { }
void chbsp_reset_release(void) { }
void chbsp_program_enable(ch_dev_t *dev_ptr) { }
void chbsp_program_disable(ch_dev_t *dev_ptr) { }
void chbsp_group_set_int1_dir_out(ch_group_t *grp_ptr) { }
void chbsp_set_int1_dir_out(ch_dev_t *dev_ptr) { }
void chbsp_group_set_int1_dir_in(ch_group_t *grp_ptr) { }
void chbsp_set_int1_dir_in(ch_dev_t *dev_ptr) { }
void chbsp_group_int1_clear(ch_group_t *grp_ptr) { }
void chbsp_group_int1_set(ch_group_t *grp_ptr) { }
void chbsp_group_int1_interrupt_enable(ch_group_t *grp_ptr) { }
void chbsp_int1_interrupt_enable(ch_dev_t *dev_ptr) { }
void chbsp_group_int1_interrupt_disable(ch_group_t *grp_ptr) { }
void chbsp_int1_interrupt_disable(ch_dev_t *dev_ptr) { }
void chbsp_int1_callback_set(ch_io_int_callback_t callback_func_ptr) { }
int chbsp_i2c_init(void) { return 0; }
uint8_t chbsp_i2c_get_info(ch_group_t __attribute__((unused)) *grp_ptr, uint8_t io_index, ch_i2c_info_t *info_ptr) { return 0; }
int chbsp_i2c_write(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 1; }
int chbsp_i2c_mem_write(ch_dev_t *dev_ptr, uint16_t mem_addr, uint8_t *data, uint16_t num_bytes) { return 1; }
int chbsp_i2c_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 1; }
int chbsp_i2c_mem_read(ch_dev_t *dev_ptr, uint16_t mem_addr, uint8_t *data, uint16_t num_bytes) { return 1; }
void chbsp_i2c_reset(ch_dev_t * dev_ptr) { }
uint8_t chbsp_periodic_timer_init(uint16_t interval_ms, ch_timer_callback_t callback_func_ptr) { return 0; }
void chbsp_periodic_timer_irq_enable(void) { }
void chbsp_periodic_timer_irq_disable(void) { }
uint8_t chbsp_periodic_timer_start(void) { return 0; }
uint8_t chbsp_periodic_timer_stop(void) { return 0; }
void chbsp_periodic_timer_handler(void) { }
void chbsp_periodic_timer_change_period(uint32_t new_period_us) { }
void chbsp_proc_sleep(void) { }
void chbsp_led_on(uint8_t led_num) { }
void chbsp_led_off(uint8_t led_num) { }
void chbsp_led_toggle(uint8_t dev_num) { }
void chbsp_print_str(char *str) { printf("%s", str); }
#ifdef INCLUDE_SHASTA_SUPPORT
void chbsp_spi_cs_on(ch_dev_t *dev_ptr) { }
void chbsp_spi_cs_off(ch_dev_t *dev_ptr) { }
int chbsp_spi_write(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
int chbsp_spi_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
#endif // INCLUDE_SHASTA_SUPPORT

#pragma GCC diagnostic pop

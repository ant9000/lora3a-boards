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
#define CHIRP_RESET_PIN         GPIO_PIN(PA, 28)
#define CHIRP_DEBUG_PIN         GPIO_PIN(PB, 23)

#define CHIRP_PROG_0             GPIO_PIN(PA, 7)
#define CHIRP_PIN_0              GPIO_PIN(PA, 6)
#define CHIRP_I2C_0              I2C_DEV(2)

static uint8_t chirp_i2c_addr[] = {41};
static uint8_t chirp_i2c_bus[] = {CHIRP_I2C_0};
uint32_t chirp_pin_prog[] = { CHIRP_PROG_0 };
uint32_t chirp_pin_io[]   = { CHIRP_PIN_0 };

#define CHIRP_NUMOF  (sizeof(chirp_i2c_addr)/sizeof(chirp_i2c_addr[0]))

// END

static void find_sensors(void) {
    uint8_t sig_bytes[2];

    chbsp_reset_release();

    /* Deassert PROG on all sensor ports */
    for (uint8_t i = 0; i < CHIRP_NUMOF; i++) {
        gpio_set(chirp_pin_prog[i]);
    }

    /* check sensors one by one */
    for (uint8_t i = 0; i < CHIRP_NUMOF; i++) {
        /* assert PROG for sensor i */
        gpio_clear(chirp_pin_prog[i]);
        printf("PROGRAM ENABLE %d\n", i);

        sig_bytes[0] = 0;
        sig_bytes[1] = 0;

        i2c_acquire(chirp_i2c_bus[i]);
        i2c_read_regs(chirp_i2c_bus[i], CH_I2C_ADDR_PROG, 0x00, sig_bytes, 2, 0);
        i2c_release(chirp_i2c_bus[i]);
        printf("Chirp sensor %d ", i);
        if ((sig_bytes[0] == CH_SIG_BYTE_0) && (sig_bytes[1] == CH_SIG_BYTE_1)) {
            printf("found\n");
        } else {
            printf("not found\n");
       }
        /* deassert PROG for sensor i */
        gpio_set(chirp_pin_prog[i]);
        printf("PROGRAM DISABLE %d\n", i);
    }
}

void chbsp_board_init(ch_group_t *grp_ptr) {
    gpio_init(LORA3A_SENSOR2_POWER, GPIO_OUT);
    gpio_init(CHIRP_RESET_PIN, GPIO_OUT);
    gpio_init(CHIRP_DEBUG_PIN, GPIO_OUT);
    for (uint8_t i = 0; i < CHIRP_NUMOF; i++) {
        gpio_init(chirp_pin_prog[i], GPIO_OUT);
    }

    /* Power on sensor busses */
    gpio_set(LORA3A_SENSOR2_POWER);
    chbsp_delay_us(5000);

    /* Initialize group descriptor */
    grp_ptr->num_ports = CHBSP_MAX_DEVICES;
    grp_ptr->num_buses = CHBSP_NUM_BUSES;
    grp_ptr->rtc_cal_pulse_ms = CHBSP_RTC_CAL_PULSE_MS;

    gpio_clear(CHIRP_DEBUG_PIN);
    chbsp_reset_assert();

    chbsp_i2c_init();

    /* Probe I2C bus to find connected sensor(s) */
    find_sensors();
}

void chbsp_delay_us(uint32_t us) {
    ztimer_sleep(ZTIMER_USEC, us);
}
void chbsp_delay_ms(uint32_t ms) {
    ztimer_sleep(ZTIMER_MSEC, ms);
}

uint32_t chbsp_timestamp_ms(void) {
    return ztimer_now(ZTIMER_MSEC);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void chbsp_reset_assert(void) {
    gpio_set(CHIRP_RESET_PIN);
puts("RESET ASSERT");
}
void chbsp_reset_release(void) {
    gpio_clear(CHIRP_RESET_PIN);
puts("RESET RELEASE");
}
void chbsp_program_enable(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    gpio_clear(chirp_pin_prog[dev_num]);
printf("PROGRAM ENABLE %d\n", dev_num);
}
void chbsp_program_disable(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    gpio_set(chirp_pin_prog[dev_num]);
printf("PROGRAM DISABLE %d\n", dev_num);
}
/*** ***/
static ch_group_t *cb_groups[CHBSP_MAX_DEVICES];
static ch_io_int_callback_t _io_int_callback=NULL;
static void _int1_callback(void *arg) {
    size_t num = (size_t)arg;
    uint8_t dev_num = (uint8_t)(num & 0xff);
    if (_io_int_callback !=NULL) {
       _io_int_callback(cb_groups[dev_num], dev_num, 0);
    }
}
void chbsp_group_set_int1_dir_out(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr))
            gpio_init(chirp_pin_io[dev_num], GPIO_OUT);
    }
}
void chbsp_set_int1_dir_out(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    gpio_init(chirp_pin_io[dev_num], GPIO_OUT);
}
void chbsp_group_set_int1_dir_in(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr))
            gpio_init(chirp_pin_io[dev_num], GPIO_IN);
    }
}
void chbsp_set_int1_dir_in(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    gpio_init(chirp_pin_io[dev_num], GPIO_IN);
}
void chbsp_group_int1_clear(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr))
            gpio_clear(chirp_pin_io[dev_num]);
    }
}
void chbsp_group_int1_set(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr))
            gpio_set(chirp_pin_io[dev_num]);
    }
}
void chbsp_group_int1_interrupt_enable(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    size_t num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            cb_groups[dev_num] = grp_ptr;
            num = dev_num;
            gpio_init_int(chirp_pin_io[dev_num], GPIO_IN, GPIO_RISING, _int1_callback, (void *)num);
        }
    }
}
void chbsp_int1_interrupt_enable(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    size_t num = dev_num;
    cb_groups[dev_num] = dev_ptr->group;
    gpio_init_int(chirp_pin_io[dev_num], GPIO_IN, GPIO_RISING, _int1_callback, (void *)num);
}
void chbsp_group_int1_interrupt_disable(ch_group_t *grp_ptr) {
    uint8_t dev_num;
    for (dev_num = 0; dev_num < ch_get_num_ports(grp_ptr); dev_num++)
    {
        ch_dev_t *dev_ptr = ch_get_dev_ptr(grp_ptr, dev_num);
        if (ch_sensor_is_connected(dev_ptr)) {
            gpio_irq_disable(chirp_pin_io[dev_num]);
        }
    }
}
void chbsp_int1_interrupt_disable(ch_dev_t *dev_ptr) {
    uint8_t dev_num = ch_get_dev_num(dev_ptr);
    gpio_irq_disable(chirp_pin_io[dev_num]);
}
void chbsp_int1_callback_set(ch_io_int_callback_t callback_func_ptr) {
    _io_int_callback = callback_func_ptr;
}
/*** ***/
int chbsp_i2c_init(void) {
    for (uint8_t i = 0; i < CHIRP_NUMOF; i++) {
        // might init same bus multiple times
        i2c_init(chirp_i2c_bus[i]);
    }
    return 0;
}
uint8_t chbsp_i2c_get_info(ch_group_t __attribute__((unused)) *grp_ptr, uint8_t io_index, ch_i2c_info_t *info_ptr) {
    uint8_t ret_val = 1;
    if (io_index <= CHBSP_MAX_DEVICES) {
        info_ptr->address = chirp_i2c_addr[io_index];
        info_ptr->bus_num = chirp_i2c_bus[io_index];
        info_ptr->drv_flags = 0;    // no special I2C handling by SonicLib driver is needed
        ret_val = 0;
    }
    return ret_val;
}
int chbsp_i2c_write(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) {
    int error;
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);
    uint8_t bus_num  = ch_get_bus(dev_ptr);
    i2c_acquire(bus_num);
    error = i2c_write_bytes(bus_num, i2c_addr, data, num_bytes, 0);
    i2c_release(bus_num);
#ifdef CHDRV_DEBUG
    printf("chbsp_i2c_write %d:0x%02x num_bytes:%d\n", bus_num, i2c_addr, num_bytes);
    printf("[ ");
    for(int i=0;i<num_bytes;i++){ printf("0x%02x ", data[i]); }
    printf("] error:%d\n", error);
#endif
    return error;
}
int chbsp_i2c_mem_write(ch_dev_t *dev_ptr, uint16_t mem_addr, uint8_t *data, uint16_t num_bytes) {
    int error;
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);
    uint8_t bus_num  = ch_get_bus(dev_ptr);
    i2c_acquire(bus_num);
    error = i2c_write_regs(bus_num, i2c_addr, mem_addr, data, num_bytes, 0);
    i2c_release(bus_num);
#ifdef CHDRV_DEBUG
    printf("chbsp_i2c_mem_write %d:0x%02x mem_addr:0x%02x num_bytes:%d\n", bus_num, i2c_addr, mem_addr, num_bytes);
    printf("[ ");
    for(int i=0;i<num_bytes;i++){ printf("0x%02x ", data[i]); }
    printf("] error:%d\n", error);
#endif
    return error;
}
int chbsp_i2c_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) {
    int error = 1;      // default is error return
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);
    uint8_t bus_num  = ch_get_bus(dev_ptr);
    i2c_acquire(bus_num);
    error = i2c_read_bytes(bus_num, i2c_addr, data, num_bytes, 0);
#ifdef CHDRV_DEBUG
    printf("chbsp_i2c_read %d:0x%02x num_bytes:%d\n", bus_num, i2c_addr, num_bytes);
    printf("[ ");
    for(int i=0;i<num_bytes;i++){ printf("0x%02x ", data[i]); }
    printf("] error:%d\n", error);
#endif
    i2c_release(bus_num);
    return error;
}
int chbsp_i2c_mem_read(ch_dev_t *dev_ptr, uint16_t mem_addr, uint8_t *data, uint16_t num_bytes) {
    int error = 1;      // default is error return
    uint8_t i2c_addr = ch_get_i2c_address(dev_ptr);
    uint8_t bus_num  = ch_get_bus(dev_ptr);
    i2c_acquire(bus_num);
chbsp_debug_on(0);
    error = i2c_read_regs(bus_num, i2c_addr, mem_addr, data, num_bytes, 0);
chbsp_debug_off(0);
#ifdef CHDRV_DEBUG
    printf("chbsp_i2c_mem_read %d:0x%02x mem_addr:0x%02x num_bytes:%d\n", bus_num, i2c_addr, mem_addr, num_bytes);
    printf("[ ");
    for(int i=0;i<num_bytes;i++){ printf("0x%02x ", data[i]); }
    printf("] error:%d\n", error);
#endif
    i2c_release(bus_num);
    return error;
}
void chbsp_i2c_reset(ch_dev_t * dev_ptr) { }
/*** ***/
uint8_t chbsp_periodic_timer_init(uint16_t interval_ms, ch_timer_callback_t callback_func_ptr) { return 0; }
void chbsp_periodic_timer_irq_enable(void) { }
void chbsp_periodic_timer_irq_disable(void) { }
uint8_t chbsp_periodic_timer_start(void) { return 0; }
uint8_t chbsp_periodic_timer_stop(void) { return 0; }
void chbsp_periodic_timer_handler(void) { }
void chbsp_periodic_timer_change_period(uint32_t new_period_us) { }
/*** ***/
void chbsp_proc_sleep(void) { }
/*** ***/
void chbsp_led_on(uint8_t led_num) { }
void chbsp_led_off(uint8_t led_num) { }
void chbsp_led_toggle(uint8_t dev_num) { }
/*** ***/
void chbsp_print_str(char *str) { printf("%s", str); }
#ifdef INCLUDE_SHASTA_SUPPORT
void chbsp_spi_cs_on(ch_dev_t *dev_ptr) { }
void chbsp_spi_cs_off(ch_dev_t *dev_ptr) { }
int chbsp_spi_write(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
int chbsp_spi_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
#endif // INCLUDE_SHASTA_SUPPORT

void chbsp_debug_toggle(uint8_t __attribute__((unused)) dbg_pin_num) {
printf("chbsp_debug_toggle %d\n", dbg_pin_num);
    gpio_toggle(CHIRP_DEBUG_PIN);
}
void chbsp_debug_on(uint8_t __attribute__((unused)) dbg_pin_num) {
printf("chbsp_debug_on %d\n", dbg_pin_num);
    gpio_set(CHIRP_DEBUG_PIN);
}
void chbsp_debug_off(uint8_t __attribute__((unused)) dbg_pin_num) {
printf("chbsp_debug_off %d\n", dbg_pin_num);
    gpio_clear(CHIRP_DEBUG_PIN);
}

#pragma GCC diagnostic pop

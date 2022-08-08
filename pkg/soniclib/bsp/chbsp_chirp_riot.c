#include "soniclib.h"
#include "chirp_bsp.h"

void chbsp_board_init(ch_group_t *grp_ptr) { }
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
void chbsp_delay_us(uint32_t us) { }
void chbsp_delay_ms(uint32_t ms) { }
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
void chbsp_print_str(char *str) { }
#ifdef INCLUDE_SHASTA_SUPPORT
void chbsp_spi_cs_on(ch_dev_t *dev_ptr) { }
void chbsp_spi_cs_off(ch_dev_t *dev_ptr) { }
int chbsp_spi_write(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
int chbsp_spi_read(ch_dev_t *dev_ptr, uint8_t *data, uint16_t num_bytes) { return 0; }
#endif // INCLUDE_SHASTA_SUPPORT

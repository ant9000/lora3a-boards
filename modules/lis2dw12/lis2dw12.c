#include <stdio.h>
#include <string.h>

#include "periph/i2c.h"
#include "ztimer.h"
#include "lis2dw12_reg.h"

#ifndef LIS2DW12_I2C_DEVICE
#define LIS2DW12_I2C_DEVICE I2C_DEV(0)
#endif
#ifndef LIS2DW12_I2C_ADDRESS
#define LIS2DW12_I2C_ADDRESS   0x19
#endif

static i2c_t bus = I2C_DEV(LIS2DW12_I2C_DEVICE);
#define SENSOR_BUS bus
#define SENSOR_ADDR LIS2DW12_I2C_ADDRESS

int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
    return i2c_write_regs(*(i2c_t *)handle, SENSOR_ADDR, reg, (uint8_t*)bufp, len, 0);
}

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    return i2c_read_regs(*(i2c_t *)handle, SENSOR_ADDR, reg, bufp, len, 0);
}

static stmdev_ctx_t dev_ctx;
static int16_t data_raw_acceleration[3], data_raw_temperature;
static uint8_t whoamI, rst;

int lis2dw12_read(float *x_mg, float *y_mg, float *z_mg, float *t_c)
{
  if (whoamI != LIS2DW12_ID) {
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.handle = &SENSOR_BUS;
    /* Wait sensor boot time */
    ztimer_sleep(ZTIMER_MSEC, 20);
    /* Check device ID */
    lis2dw12_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LIS2DW12_ID) {
      puts("LIS2DW12 not found");
      return -1;
    }

    /* Restore default configuration */
    lis2dw12_reset_set(&dev_ctx, PROPERTY_ENABLE);

    do {
      lis2dw12_reset_get(&dev_ctx, &rst);
    } while (rst);

    /* Enable Block Data Update */
    lis2dw12_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    /* Set full scale */
    //lis2dw12_full_scale_set(&dev_ctx, LIS2DW12_8g);
    lis2dw12_full_scale_set(&dev_ctx, LIS2DW12_2g);
    /* Configure filtering chain
     * Accelerometer - filter path / bandwidth
     */
    lis2dw12_filter_path_set(&dev_ctx, LIS2DW12_LPF_ON_OUT);
    lis2dw12_filter_bandwidth_set(&dev_ctx, LIS2DW12_ODR_DIV_4);
    /* Configure power mode */
    lis2dw12_power_mode_set(&dev_ctx, LIS2DW12_HIGH_PERFORMANCE);
    //lis2dw12_power_mode_set(&dev_ctx, LIS2DW12_CONT_LOW_PWR_LOW_NOISE_12bit);
    /* Set Output Data Rate */
    lis2dw12_data_rate_set(&dev_ctx, LIS2DW12_XL_ODR_25Hz);
  }

  uint8_t reg = 0;
  while (!reg) {
    /* Read output only if new value is available */
    lis2dw12_flag_data_ready_get(&dev_ctx, &reg);
  }

  /* Read acceleration data */
  memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
  lis2dw12_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
  *x_mg = lis2dw12_from_fs2_to_mg(data_raw_acceleration[0]);
  *y_mg = lis2dw12_from_fs2_to_mg(data_raw_acceleration[1]);
  *z_mg = lis2dw12_from_fs2_to_mg(data_raw_acceleration[2]);
  /* Read temperature data */
  data_raw_temperature=0;
  lis2dw12_temperature_raw_get(&dev_ctx, &data_raw_temperature);
  *t_c = lis2dw12_from_lsb_to_celsius(data_raw_temperature);

  return 0;
}

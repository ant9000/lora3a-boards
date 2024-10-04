#include <stdio.h>

#include "board.h"

#ifdef FRAM_PARAM_I2C
#define AT24CXXX_PARAM_I2C  FRAM_PARAM_I2C
#endif

#ifdef FRAM_ENABLE_PIN
#ifndef FRAM_ENABLE_ON
#define FRAM_ENABLE_ON (1)
#endif
#endif


#include "at24cxxx.h"
#include "at24cxxx_params.h"
#define ENABLE_DEBUG 0

#include "debug.h"

static at24cxxx_t at24cxxx_dev;

void fram_on(void)
{
#if defined(FRAM_ENABLE_PIN)
    if (gpio_is_valid(FRAM_ENABLE_PIN)) {
        gpio_write(FRAM_ENABLE_PIN, FRAM_ENABLE_ON);
    }
#endif
}

void fram_off(void)
{
#if defined(FRAM_ENABLE_PIN)
    if (gpio_is_valid(FRAM_ENABLE_PIN)) {
        gpio_write(FRAM_ENABLE_PIN, 1-FRAM_ENABLE_ON);
    }
#endif
}

int fram_init(void)
{
    DEBUG("FRAM size: %u byte\n", AT24CXXX_EEPROM_SIZE);
    DEBUG("Page size  : %u byte\n", AT24CXXX_PAGE_SIZE);
#if defined(FRAM_ENABLE_PIN)
    if (gpio_is_valid(FRAM_ENABLE_PIN)) {
        gpio_init(FRAM_ENABLE_PIN, GPIO_OUT);
    }
#endif
    fram_on();
    int status = at24cxxx_init(&at24cxxx_dev, &at24cxxx_params[0]);
    if (status != AT24CXXX_OK) {
        printf("ERROR: FRAM initialization failed (%d)\n", status);
        return 1;
    }
    DEBUG("FRAM initialized successfully");
    return 0;
}

int fram_read(uint32_t pos, void *data, size_t len)
{
    if (pos + len > AT24CXXX_EEPROM_SIZE) {
        puts("ERROR: cannot read out of FRAM bounds");
        return 1;
    }
    fram_on();
    int status = at24cxxx_read(&at24cxxx_dev, pos, data, len);
    if (status != AT24CXXX_OK) {
        printf("ERROR: read from FRAM failed (%d)\n", status);
        return 1;
    }
    return 0;
}

int fram_write(uint32_t pos, uint8_t *data, size_t len)
{
    if (pos + len > AT24CXXX_EEPROM_SIZE) {
        puts("ERROR: cannot write out of FRAM bounds");
        return 1;
    }
    fram_on();
    int status = at24cxxx_write(&at24cxxx_dev, pos, data, len);
    if (status != AT24CXXX_OK) {
        printf("ERROR: write to FRAM failed (%d)\n", status);
        return 1;
    }
    return 0;
}

int fram_erase(void)
{
    fram_on();
    int status = at24cxxx_erase(&at24cxxx_dev);
    if (status != AT24CXXX_OK) {
        printf("ERROR: FRAM erase failed (%d)\n", status);
        return 1;
    }
    DEBUG("FRAM erased");
    return 0;
}

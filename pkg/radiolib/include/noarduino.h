#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <Arduino.h>

typedef enum {
  FALLING = 3,
  RISING  = 4,
} PinStatus;

// the following parameters must always be defined
#define RADIOLIB_PIN_TYPE                           uint8_t
#define RADIOLIB_PIN_MODE                           uint8_t
#define RADIOLIB_PIN_STATUS                         uint8_t
#define RADIOLIB_INTERRUPT_STATUS                   RADIOLIB_PIN_STATUS
#define RADIOLIB_DIGITAL_PIN_TO_INTERRUPT(p)        digitalPinToInterrupt(p)
#define RADIOLIB_NC                                 (0xFF)
#define RADIOLIB_DEFAULT_SPI                        SPI
#define RADIOLIB_DEFAULT_SPI_SETTINGS               SPISettings(2000000, MSBFIRST, SPI_MODE0)
#define RADIOLIB_NONVOLATILE                        // unset
//#define RADIOLIB_NONVOLATILE_READ_BYTE(addr)        pgm_read_byte(addr)
#define RADIOLIB_TYPE_ALIAS(type, alias)            using alias = type;

// Arduino API callbacks
// the following are signatures of Arduino API functions of the custom platform
// for example, pinMode on Arduino Uno is defined as void pinMode(uint8_t pin, uint8_t mode)
// all of the callbacks below are taken from Arduino Uno
#define RADIOLIB_CB_ARGS_PIN_MODE                   (void, pinMode, uint8_t pin, uint8_t mode)
#define RADIOLIB_CB_ARGS_DIGITAL_WRITE              (void, digitalWrite, uint8_t pin, uint8_t value)
#define RADIOLIB_CB_ARGS_DIGITAL_READ               (int, digitalRead, uint8_t pin)
#define RADIOLIB_CB_ARGS_TONE                       (void, tone, uint8_t _pin, unsigned int frequency, unsigned long duration)
#define RADIOLIB_CB_ARGS_NO_TONE                    (void, noTone, uint8_t _pin)
#define RADIOLIB_CB_ARGS_ATTACH_INTERRUPT           (void, attachInterrupt, uint8_t interruptNum, void (*userFunc)(void), int mode)
#define RADIOLIB_CB_ARGS_DETACH_INTERRUPT           (void, detachInterrupt, uint8_t interruptNum)
#define RADIOLIB_CB_ARGS_YIELD                      (void, yield, void)
#define RADIOLIB_CB_ARGS_DELAY                      (void, delay, unsigned long ms)
#define RADIOLIB_CB_ARGS_DELAY_MICROSECONDS         (void, delayMicroseconds, unsigned int us)
#define RADIOLIB_CB_ARGS_MILLIS                     (unsigned long, millis, void)
#define RADIOLIB_CB_ARGS_MICROS                     (unsigned long, micros, void)
#define RADIOLIB_CB_ARGS_PULSE_IN                   (unsigned long, pulseIn, uint8_t pin, uint8_t state, unsigned long timeout)
#define RADIOLIB_CB_ARGS_SPI_BEGIN                  (void, SPIbegin, void)
#define RADIOLIB_CB_ARGS_SPI_BEGIN_TRANSACTION      (void, SPIbeginTransaction, void)
#define RADIOLIB_CB_ARGS_SPI_TRANSFER               (uint8_t, SPItransfer, uint8_t b)
#define RADIOLIB_CB_ARGS_SPI_END_TRANSACTION        (void, SPIendTransaction, void)
#define RADIOLIB_CB_ARGS_SPI_END                    (void, SPIend, void)

// the following must be defined if the Arduino core does not support tone function
#define RADIOLIB_TONE_UNSUPPORTED

// some platforms seem to have issues with SPI modules that use a command interface
// this can be mitigated by adding delays into SPI communication
// (see https://github.com/jgromes/RadioLib/issues/158 for details)
//#define RADIOLIB_SPI_SLOWDOWN

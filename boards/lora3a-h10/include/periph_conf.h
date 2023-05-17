#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MODE_USART 1
#define MODE_SPI   2
#define MODE_I2C   3

/**
 * @brief   GCLK reference speed
 */
#ifdef MODULE_PERIPH_USBDEV
#define CLOCK_CORECLOCK     (48000000U)
#else
#define CLOCK_CORECLOCK     (16000000U)
#endif

/**
 * @brief Enable the internal DC/DC converter
 *        The board is equipped with the necessary inductor.
 */
#define USE_VREG_BUCK       (1)

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBCMASK.reg,
        .mclk_mask      = MCLK_APBCMASK_TC0 | MCLK_APBCMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    },
    {
        .dev            = TC2,
        .irq            = TC2_IRQn,
        .mclk           = &MCLK->APBCMASK.reg,
        .mclk_mask      = MCLK_APBCMASK_TC2 | MCLK_APBCMASK_TC3,
        .gclk_id        = TC2_GCLK_ID,
        .gclk_src       = SAM0_GCLK_TIMER,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0
#define TIMER_1_CHANNELS    2
#define TIMER_1_ISR         isr_tc2
#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM3->USART,
        .rx_pin   = GPIO_PIN(PA, 22),
        .tx_pin   = GPIO_PIN(PA, 23),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
#if ENABLE_ACME1 == MODE_USART
    },
    {    /* ACME1 USART */
        .dev      = &SERCOM5->USART,
        .rx_pin   = GPIO_PIN(PB, 3),
        .tx_pin   = GPIO_PIN(PB, 2),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
#endif
#if ENABLE_ACME2 == MODE_USART
    },
    {
        .dev      = &(SERCOM0->USART),
        .rx_pin  = GPIO_PIN(PA, 5),
        .tx_pin  = GPIO_PIN(PA, 4),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_D,
        .rx_pad   = UART_PAD_RX_1,
        .tx_pad   = UART_PAD_TX_0,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
#endif
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom3
#if ENABLE_ACME1 == MODE_USART
#define UART_1_ISR          isr_sercom5
#if ENABLE_ACME2 == MODE_USART
#define UART_2_ISR          isr_sercom0
#endif
#else
#if ENABLE_ACME2 == MODE_USART
#define UART_1_ISR          isr_sercom0
#endif
#endif

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = &(SERCOM4->SPI),
        .miso_pin = GPIO_PIN(PC, 19),
        .mosi_pin = GPIO_PIN(PB, 30),
        .clk_pin  = GPIO_PIN(PC, 18),
        .miso_mux = GPIO_MUX_F,
        .mosi_mux = GPIO_MUX_F,
        .clk_mux  = GPIO_MUX_F,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM4_DMAC_ID_TX,
        .rx_trigger = SERCOM4_DMAC_ID_RX,
#endif
#if ENABLE_ACME1 == MODE_SPI
    },
    {
        .dev      = &(SERCOM5->SPI),
        .miso_pin = GPIO_PIN(PB, 3),
        .mosi_pin = GPIO_PIN(PB, 2),
        .clk_pin  = GPIO_PIN(PB, 23),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_1,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM5_DMAC_ID_TX,
        .rx_trigger = SERCOM5_DMAC_ID_RX,
#endif
#endif
#if ENABLE_ACME2 == MODE_SPI
    },
    {
        .dev      = &(SERCOM0->SPI),
        .miso_pin = GPIO_PIN(PA, 5),
        .mosi_pin = GPIO_PIN(PA, 4),
        .clk_pin  = GPIO_PIN(PA, 7),
        .miso_mux = GPIO_MUX_D,
        .mosi_mux = GPIO_MUX_D,
        .clk_mux  = GPIO_MUX_D,
        .miso_pad = SPI_PAD_MISO_1,
        .mosi_pad = SPI_PAD_MOSI_0_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        .tx_trigger = SERCOM0_DMAC_ID_TX,
        .rx_trigger = SERCOM0_DMAC_ID_RX,
#endif
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM1->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 17),  // Bus interno
        .sda_pin  = GPIO_PIN(PA, 16),  // Bus interno
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
#if ENABLE_ACME1 == MODE_I2C
    },
    {
        .dev      = &(SERCOM5->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PB, 3),
        .sda_pin  = GPIO_PIN(PB, 2),
        .mux      = GPIO_MUX_D,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
#endif
#if ENABLE_ACME2 == MODE_I2C
    },
    {
        .dev      = &(SERCOM0->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 5),
        .sda_pin  = GPIO_PIN(PA, 4),
        .mux      = GPIO_MUX_D,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
#endif
     }
};
#define I2C_NUMOF          ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                   0
#define INTERNAL_OSC32_SOURCE                   0
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE     1
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (1024U)
#endif
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC 0 Default values */
#define ADC_PRESCALER                           ADC_CTRLB_PRESCALER_DIV256

#define ADC_NEG_INPUT                           ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                         ADC_REFCTRL_REFSEL_INTREF // ADC_REFCTRL_REFSEL_INTVCC2 // ADC_REFCTRL_REFSEL_INTREF

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_SCALEDIOVCC }, // mux pin is unused
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA08 },        // Vpanel
#if defined(RESISTOR)
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_PA04 },        // Resistor
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_BANDGAP_Val }, // Bandgap
    { .inputctrl = ADC_INPUTCTRL_MUXPOS_TEMP },        // Temperature
#endif
};


#define ADC_NUMOF                               ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name USB peripheral configuration
 * @{

static const sam0_common_usb_config_t sam_usbdev_config[] = {
    {
        .dm       = GPIO_PIN(PA, 24),
        .dp       = GPIO_PIN(PA, 25),
        .d_mux    = GPIO_MUX_G,
        .device   = &USB->DEVICE,
        .gclk_src = SAM0_GCLK_48MHZ,
    }
};
*/
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

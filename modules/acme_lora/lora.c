#include "thread.h"
#include "mutex.h"
#include "net/netdev/lora.h"

#include "sx127x_internal.h"
#include "sx127x_params.h"
#include "sx127x_netdev.h"

#include "common.h"

#define SX127X_LORA_MSG_QUEUE   (32U)
#define SX127X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#define MSG_TYPE_ISR            (0x3456)

#define ENABLE_DEBUG 0

#include "debug.h"

static char stack[SX127X_STACKSIZE];
static kernel_pid_t lora_recv_pid;
static sx127x_t sx127x;
static char lora_buffer[MAX_PACKET_LEN];
static netdev_lora_rx_info_t lora_packet_info;

static lora_data_cb_t *lora_data_cb;
static void _lora_event_cb(netdev_t *dev, netdev_event_t event);
void *_lora_recv_thread(void *arg);

static mutex_t lora_transmission_lock = MUTEX_INIT;

static bool lora_init_done = false;

static const lora_state_t *lora;

int lora_init(const lora_state_t *state)
{
	lora = state;
    lora_data_cb = state->data_cb;

    sx127x.params = sx127x_params[0];
    netdev_t *netdev = (netdev_t *)&sx127x;
    netdev->event_callback = _lora_event_cb;
    netdev->driver = &sx127x_driver;
#if defined(BOARD_SAMR34_XPRO) || defined (BOARD_LORA3A_H10)
        gpio_init(TCXO_PWR_PIN, GPIO_OUT);
        gpio_set(TCXO_PWR_PIN);
        gpio_init(TX_OUTPUT_SEL_PIN, GPIO_OUT);
        gpio_write(TX_OUTPUT_SEL_PIN, SX127X_PARAM_PASELECT);  // set TX_OUTPUT_SEL_PIN at "1" for Rx
#endif

    spi_init(sx127x.params.spi);
    if (netdev->driver->init(netdev) < 0) return 1;

    uint8_t  lora_bw = state->bandwidth;
    uint8_t  lora_sf = state->spreading_factor;
    uint8_t  lora_cr = state->coderate;
    uint32_t lora_ch = state->channel;
    int16_t  lora_pw = state->power;

    netdev->driver->set(netdev, NETOPT_BANDWIDTH, &lora_bw, sizeof(lora_bw));
    netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR, &lora_sf, sizeof(lora_sf));
    netdev->driver->set(netdev, NETOPT_CODING_RATE, &lora_cr, sizeof(lora_cr));
    netdev->driver->set(netdev, NETOPT_CHANNEL_FREQUENCY, &lora_ch, sizeof(lora_ch));
    netdev->driver->set(netdev, NETOPT_TX_POWER, &lora_pw, sizeof(lora_pw));

    lora_recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1,
                              THREAD_CREATE_STACKTEST, _lora_recv_thread, NULL,
                              "_lora_recv_thread");

    if (lora_recv_pid <= KERNEL_PID_UNDEF) return 1;
    lora_init_done = true;
    return 0;
}

void lora_off(void)
{
    if (!lora_init_done) {
        sx127x.params = sx127x_params[0];
        spi_init(sx127x.params.spi);
        sx127x_init(&sx127x);
    }
    sx127x_set_sleep(&sx127x);
    spi_release(sx127x.params.spi);
    spi_deinit_pins(sx127x.params.spi);
#if defined(BOARD_SAMR34_XPRO) || defined (BOARD_LORA3A_H10)
        gpio_clear(TCXO_PWR_PIN);
        gpio_clear(TX_OUTPUT_SEL_PIN);
#endif
}

int lora_write(const iolist_t *packet)
{
    netdev_t *netdev = (netdev_t *)&sx127x;
    uint8_t len = iolist_size(packet);
    mutex_lock(&lora_transmission_lock);
#if defined(BOARD_SAMR34_XPRO) || defined (BOARD_LORA3A_H10)
	printf("boost=%d txpower=%d\n",lora->boost, lora->power);
// put here the output select pin selection based on persist value
	if (lora->boost) {
		gpio_clear(TX_OUTPUT_SEL_PIN);   // V1 = 0
	} else {
		gpio_set(TX_OUTPUT_SEL_PIN);  // V1 = 1
    }
#endif    
    if (netdev->driver->send(netdev, packet) == -ENOTSUP) {
        puts("TX FAILED");
        len = -1;
    } else {
        // wait for end of transmission
        uint32_t delay = sx127x_get_time_on_air(&sx127x, len);  // it seems that time_on_air is not very accurate. To be investigated better
        if (ztimer_mutex_lock_timeout(ZTIMER_USEC, &lora_transmission_lock, delay * 1000 + 50) != 0) { // ex + 10 
            puts("TX TIMEOUT");
            len = -1;
        }
    }
#if defined(BOARD_SAMR34_XPRO) || defined (BOARD_LORA3A_H10)
	gpio_set(TX_OUTPUT_SEL_PIN);  // V1 = 1  for Rx
#endif
    mutex_unlock(&lora_transmission_lock);
    return len;
}

void lora_listen(void)
{
    netdev_t *netdev = (netdev_t *)&sx127x;
    const netopt_enable_t single = false;
    netdev->driver->set(netdev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));
    const uint32_t timeout = 0;
    netdev->driver->set(netdev, NETOPT_RX_TIMEOUT, &timeout, sizeof(timeout));
    netopt_state_t state = NETOPT_STATE_RX;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));
}

uint8_t lora_get_power(void)
{
    return sx127x_get_tx_power(&sx127x);
}

void lora_set_power(uint8_t power)
{
    sx127x_set_tx_power(&sx127x, power);
}

static void _lora_event_cb(netdev_t *dev, netdev_event_t event)
{
    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;
        msg.type = MSG_TYPE_ISR;
        msg.content.ptr = dev;
        if (msg_send(&msg, lora_recv_pid) <= 0) {
           /* possibly lost interrupt */
        }
    } else {
        size_t len;
        switch (event) {
            case NETDEV_EVENT_RX_COMPLETE:
                len = dev->driver->recv(dev, NULL, 0, 0);
                DEBUG("NETDEV_EVENT_RX_COMPLETE: %d bytes\n", len);
                if (len <= MAX_PACKET_LEN) {
                    dev->driver->recv(dev, lora_buffer, len, &lora_packet_info);
                    lora_data_cb(lora_buffer, len, &lora_packet_info.rssi, &lora_packet_info.snr);
                } else {
                    /* spurious communication - ignore */
                }
                break;
            case NETDEV_EVENT_TX_COMPLETE:
                DEBUG("NETDEV_EVENT_TX_COMPLETE");
                mutex_unlock(&lora_transmission_lock);
                break;
            default:
                DEBUG("NETDEV_EVENT #%d\n", event);
                break;
        }
    }
}

void *_lora_recv_thread(void *arg)
{
    (void)arg;
    static msg_t _msg_q[SX127X_LORA_MSG_QUEUE];
    msg_init_queue(_msg_q, SX127X_LORA_MSG_QUEUE);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_ISR) {
            netdev_t *dev = msg.content.ptr;
            dev->driver->isr(dev);
        }
    }
}

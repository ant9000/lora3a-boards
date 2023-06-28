#include <stdio.h>

#include "acme_lora.h"
#include "fmt.h"
#include "mutex.h"
#include "net/netdev.h"
#include "od.h"

#ifdef CONFIG_AES
#include <string.h>

#include "aes.h"
#include "periph/hwrng.h"
#ifndef AES_KEY
#define AES_KEY "a146e090b068f4a4edf6dc5812930265"
#endif
#endif

static consume_data_cb_t *protocol_packet_consumer;

static mutex_t lora_lock = MUTEX_INIT;

#ifdef CONFIG_AES
static struct aes_sync_device aes;
static uint8_t aes_key[16];
#endif

#define ENABLE_DEBUG 0
#include "debug.h"
#define HEXDUMP(msg, buffer, len)        \
  if (ENABLE_DEBUG) {                    \
    puts(msg);                           \
    od_hex_dump((char *)buffer, len, 0); \
  }

void protocol_init(consume_data_cb_t *packet_consumer) {
  protocol_packet_consumer = packet_consumer;
#ifdef CONFIG_AES
  hwrng_init();
  fmt_hex_bytes(aes_key, AES_KEY);
  aes_sync_set_encrypt_key(&aes, aes_key, AES_KEY_128);
#endif
}

void protocol_in(const char *buffer, size_t len, int16_t *rssi, int8_t *snr) {
  embit_header_t *header;
  char *payload;
  size_t n;
  mutex_lock(&lora_lock);
  header = (embit_header_t *)(void const *)buffer;
  HEXDUMP("RECEIVED PACKET:", buffer, len);
  if ((header->signature == EMB_SIGNATURE) && (len > EMB_HEADER_LEN)) {
    payload = (char *)buffer + EMB_HEADER_LEN;
    n = len - EMB_HEADER_LEN;
#ifndef CONFIG_AES
    embit_packet_t packet = {.header = *header,
                             .payload = payload,
                             .payload_len = n,
                             .rssi = *rssi,
                             .snr = *snr};
    protocol_packet_consumer(&packet);
#else
    if (n > 12 + 16) {
      uint8_t aes_output[MAX_PACKET_LEN];
      n -= 12 + 16;
      HEXDUMP("RECEIVED PAYLOAD:", payload, n);
      uint8_t *nonce = (uint8_t *)payload + n;
      HEXDUMP("RECEIVED NONCE:", nonce, 12);
      uint8_t *tag = nonce + 12;
      HEXDUMP("RECEIVED TAG:", tag, 16);
      uint8_t verify[16];
      aes_sync_gcm_crypt_and_tag(&aes, AES_DECRYPT, (uint8_t *)payload,
                                 aes_output, n, nonce, 12, (uint8_t *)header,
                                 EMB_HEADER_LEN, verify, 16);
      if (memcmp(tag, verify, 16) == 0) {
        // remove padding
        char c = aes_output[n - 1];
        if (c <= 16) {
          n -= c;
        }
        embit_packet_t packet = {.header = *header,
                                 .payload = (char *)aes_output,
                                 .payload_len = n,
                                 .rssi = *rssi,
                                 .snr = *snr};
        protocol_packet_consumer(&packet);
      }
    }
#endif
  }
  mutex_unlock(&lora_lock);
}

void protocol_out(const embit_header_t *header, const char *buffer,
                  const size_t len) {
  iolist_t packet, payload;
  mutex_lock(&lora_lock);
  packet.iol_base = (void *)header;
  packet.iol_len = EMB_HEADER_LEN;
  packet.iol_next = &payload;
#ifndef CONFIG_AES
  payload.iol_base = (void *)buffer;
  payload.iol_len = len;
#else
  uint8_t aes_input[MAX_PACKET_LEN], aes_output[MAX_PACKET_LEN];
  size_t n = len;
  uint8_t nonce[12];
  uint8_t tag[16];
  hwrng_read(nonce, 12);
  // pad input to a multiple of 128 bits using PKCS#7
  memcpy(aes_input, (uint8_t *)buffer, len);
  char c = 16 - (len % 16);
  memset(aes_input + len, c, c);
  n += c;
  HEXDUMP("PADDED PACKET:", aes_input, n);
  aes_sync_gcm_crypt_and_tag(&aes, AES_ENCRYPT, aes_input, aes_output, n, nonce,
                             12, (uint8_t *)header, EMB_HEADER_LEN, tag, 16);
  HEXDUMP("NONCE:", nonce, 12);
  memcpy(aes_output + n, nonce, 12);
  n += 12;
  HEXDUMP("TAG:", tag, 16);
  memcpy(aes_output + n, tag, 16);
  n += 16;
  HEXDUMP("ENCRYPTED PACKET:", aes_output, n);
  payload.iol_base = (void *)aes_output;
  payload.iol_len = n;
#endif
  payload.iol_next = NULL;
  lora_write(&packet);
  mutex_unlock(&lora_lock);
}

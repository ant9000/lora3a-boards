#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <inttypes.h>
#include <stddef.h>

#include "acme_lora.h"
#include "net/netdev.h"

typedef struct {
  uint16_t signature;
  uint16_t counter;
  uint16_t network;
  uint8_t dst;
  uint8_t src;
} acme_header_t;

typedef struct {
  acme_header_t header;
  char *payload;
  size_t payload_len;
  int16_t rssi;
  int8_t snr;
} acme_packet_t;

typedef ssize_t(consume_data_cb_t)(const acme_packet_t *packet);

#define ACME_HEADER_LEN sizeof(acme_header_t)

#define ACME_BROADCAST 0xff
#define ACME_SIGNATURE 0x00e0

void protocol_init(consume_data_cb_t *packet_consumer);
void protocol_in(const char *buffer, size_t len, int16_t *rssi, int8_t *snr);
void protocol_out(const acme_header_t *header, const char *buffer,
                  const size_t len);

#endif /* PROTOCOL_H */

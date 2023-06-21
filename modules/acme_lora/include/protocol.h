#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "net/netdev.h"

void protocol_init(consume_data_cb_t *packet_consumer);
void protocol_in(const char *buffer, size_t len, int16_t *rssi, int8_t *snr);
void protocol_out(const embit_header_t *header, const char *buffer, const size_t len);

#endif /* PROTOCOL_H */

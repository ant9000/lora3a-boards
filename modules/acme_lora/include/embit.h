#ifndef EMBIT_H
#define EMBIT_H

#include <inttypes.h>

typedef struct {
    uint16_t signature;
    uint16_t counter;
    uint16_t network;
    uint8_t dst;
    uint8_t src;
} embit_header_t;

typedef struct {
    embit_header_t header;
    char *payload;
    size_t payload_len;
    int16_t rssi;
    int8_t snr;
} embit_packet_t;

#define EMB_HEADER_LEN  sizeof(embit_header_t)

#define EMB_BROADCAST 0xff
#define EMB_SIGNATURE 0x00e0

#endif /* EMBIT_H */

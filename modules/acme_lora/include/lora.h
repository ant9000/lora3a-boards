#ifndef LORA_H
#define LORA_H

#include "net/netdev.h"

typedef void(lora_data_cb_t)(const char *buffer, size_t len, int16_t *rssi,
                             int8_t *snr);

typedef struct {
  uint8_t bandwidth;
  uint8_t spreading_factor;
  uint8_t coderate;
  uint32_t channel;
  int16_t power;
  uint8_t boost;
  lora_data_cb_t *data_cb;
} lora_state_t;

int lora_init(const lora_state_t *state);
void lora_off(void);
uint8_t lora_get_power(void);
void lora_set_power(uint8_t power);
int lora_write(const iolist_t *packet);
void lora_listen(void);

#endif /* LORA_H */

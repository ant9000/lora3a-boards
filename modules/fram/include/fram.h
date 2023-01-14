#ifndef FRAM_H
#define FRAM_H

#include <inttypes.h>

void fram_on(void);
void fram_off(void);
int fram_init(void);
int fram_read(uint32_t pos, void *data, size_t len);
int fram_write(uint32_t pos, uint8_t *data, size_t len);
int fram_erase(void);

#endif

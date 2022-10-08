#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "od.h"
#include "shell.h"

#include "at24cxxx.h"
#include "at24cxxx_params.h"

#define BUFFER_SIZE     (128U)

static char buffer[BUFFER_SIZE + 1];
static at24cxxx_t at24cxxx_dev;
static int initialized = 0;

static int cmd_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("EEPROM size: %u byte\n", AT24CXXX_EEPROM_SIZE);
    printf("Page size  : %u byte\n", AT24CXXX_PAGE_SIZE);

    int check = at24cxxx_init(&at24cxxx_dev, &at24cxxx_params[0]);
    if (check != AT24CXXX_OK) {
        printf("ERROR: initialization failed (%d)\n", check);
        return 1;
    }
    puts("FRAM initialized successfully");
    initialized = 1;
    return 0;
}

static int cmd_read(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <pos> <count>\n", argv[0]);
        return 1;
    }

    if (!initialized) {
        puts("FRAM must be initialized first");
        return 1;
    }

    uint32_t pos = atoi(argv[1]);
    uint32_t count = atoi(argv[2]);

    if (count > BUFFER_SIZE) {
        printf("Count exceeds buffer size (%d)", BUFFER_SIZE);
        return 1;
    }

    if (pos + count > AT24CXXX_EEPROM_SIZE) {
        puts("Failed: cannot read out of FRAM bounds");
        return 1;
    }

    int check = at24cxxx_read(&at24cxxx_dev, pos, buffer, count);
    buffer[count] = '\0';
    if (check != AT24CXXX_OK) {
        printf("ERROR: read from FRAM failed (%d)\n", check);
        return 1;
    }

    printf("Data read from FRAM (%lu bytes):\n", count);
    od_hex_dump(buffer, count, 0);

    return 0;
}

static int cmd_write(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <pos> <data>\n", argv[0]);
        return 1;
    }

    if (!initialized) {
        puts("FRAM must be initialized first");
        return 1;
    }

    uint32_t pos = atoi(argv[1]);
    uint8_t *data = (uint8_t *)argv[2];
    size_t len = strlen(argv[2]);

    if (pos + strlen(argv[2]) > AT24CXXX_EEPROM_SIZE) {
        puts("Failed: cannot write out of FRAM bounds");
        return 1;
    }

    int check = at24cxxx_write(&at24cxxx_dev, pos, data, len);
    if (check != AT24CXXX_OK) {
        printf("ERROR: write to FRAM failed (%d)\n", check);
        return 1;
    }
    printf("%d bytes written to FRAM\n", len);

    return 0;
}

static int cmd_erase(int argc, char **argv)
{
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }

    int check = at24cxxx_erase(&at24cxxx_dev);
    if (check != AT24CXXX_OK) {
        printf("ERROR: FRAM erase failed (%d)\n", check);
        return 1;
    }
    puts("FRAM erased with success");

    return 0;
}

SHELL_COMMAND(fram_init, "Initialize FRAM", cmd_init);
SHELL_COMMAND(fram_read, "Read bytes from FRAM", cmd_read);
SHELL_COMMAND(fram_write, "Write bytes to FRAM", cmd_write);
SHELL_COMMAND(fram_erase, "Erase whole FRAM", cmd_erase);

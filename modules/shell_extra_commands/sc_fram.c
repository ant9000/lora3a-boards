#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "od.h"
#include "shell.h"

#include "fram.h"

#define BUFFER_SIZE     (128U)

static char buffer[BUFFER_SIZE];
static int initialized = 0;

static int cmd_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    initialized = (fram_init() == 0);
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

    for(uint32_t len = 0; len <= count; len += BUFFER_SIZE) {
        uint32_t n = (count - len) > BUFFER_SIZE ? BUFFER_SIZE : (count - len);
        if (fram_read(pos + len, buffer, n)) {
            return 1;
        }
        od_hex_dump_ext(buffer, n, 0, len);
    }
    printf("\n%lu bytes read from FRAM\n", count);
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

    if (fram_write(pos, data, len)) {
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

    if (fram_erase()) {
        return 1;
    }
    return 0;
}

SHELL_COMMAND(fram_init, "Initialize FRAM", cmd_init);
SHELL_COMMAND(fram_read, "Read bytes from FRAM", cmd_read);
SHELL_COMMAND(fram_write, "Write bytes to FRAM", cmd_write);
SHELL_COMMAND(fram_erase, "Erase whole FRAM", cmd_erase);

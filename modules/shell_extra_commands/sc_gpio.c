/*
 * Copyright (C) 2014,2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "periph/gpio.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
static void cb(void *arg)
{
    printf("INT: external interrupt from pin %i\n", (int)arg);
}
#endif

static int cmd_gpio_init_pin(int argc, char **argv)
{
    int po, pi;
    gpio_mode_t mode;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <mode>\n", argv[0]);
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    if (strcmp(argv[3],"OUT")==0) {
        mode = GPIO_OUT;
    } else if (strcmp(argv[3],"IN")==0) {
        mode = GPIO_IN;
    } else if (strcmp(argv[3],"IN_PU")==0) {
        mode = GPIO_IN_PU;
    } else if (strcmp(argv[3],"IN_PD")==0) {
        mode = GPIO_IN_PD;
    } else if (strcmp(argv[3],"OD")==0) {
        mode = GPIO_OD;
    } else if (strcmp(argv[3],"OD_PU")==0) {
        mode = GPIO_OD_PU;
    } else {
        printf("unknown mode '%s' - should be one of OUT, IN, IN_PU, IN_PD, OD, OD_PU\n", argv[3]);
        return 1;
    }

    if (gpio_init(GPIO_PIN(po, pi), mode) < 0) {
        printf("Error to initialize GPIO_PIN(%i, %02i)\n", po, pi);
        return 1;
    }

    return 0;
}
SHELL_COMMAND(gpio_init_pin, "init gpio pin", cmd_gpio_init_pin);

#ifdef MODULE_PERIPH_GPIO_IRQ
static int cmd_gpio_init_int(int argc, char **argv)
{
    int po, pi;
    gpio_mode_t mode = GPIO_IN;
    gpio_flank_t flank;
    int fl;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <flank> [pull_config]\n", argv[0]);
        puts("\tflank:\n"
             "\t0: falling\n"
             "\t1: rising\n"
             "\t2: both\n"
             "\tpull_config:\n"
             "\t0: no pull resistor (default)\n"
             "\t1: pull up\n"
             "\t2: pull down");
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    fl = atoi(argv[3]);
    switch (fl) {
        case 0:
            flank = GPIO_FALLING;
            break;
        case 1:
            flank = GPIO_RISING;
            break;
        case 2:
            flank = GPIO_BOTH;
            break;
        default:
            puts("error: invalid value for active flank");
            return 1;
    }

    if (argc >= 5) {
        int pr = atoi(argv[4]);
        switch (pr) {
            case 0:
                mode = GPIO_IN;
                break;
            case 1:
                mode = GPIO_IN_PU;
                break;
            case 2:
                mode = GPIO_IN_PD;
                break;
            default:
                puts("error: invalid pull resistor option");
                return 1;
        }
    }

    if (gpio_init_int(GPIO_PIN(po, pi), mode, flank, cb, (void *)pi) < 0) {
        printf("error: init_int of GPIO_PIN(%i, %i) failed\n", po, pi);
        return 1;
    }
    printf("GPIO_PIN(%i, %i) successfully initialized as ext int\n", po, pi);

    return 0;
}
SHELL_COMMAND(gpio_init_int, "init as external INT w/o pull resistor", cmd_gpio_init_int);

static int cmd_gpio_enable_int(int argc, char **argv)
{
    int po, pi;
    int status;

    if (argc < 4) {
        printf("usage: %s <port> <pin> <status>\n", argv[0]);
        puts("\tstatus:\n"
             "\t0: disable\n"
             "\t1: enable\n");
        return 1;
    }

    po = atoi(argv[1]);
    pi = atoi(argv[2]);

    status = atoi(argv[3]);

    switch (status) {
        case 0:
            puts("disabling GPIO interrupt");
            gpio_irq_disable(GPIO_PIN(po, pi));
            break;
        case 1:
            puts("enabling GPIO interrupt");
            gpio_irq_enable(GPIO_PIN(po, pi));
            break;
        default:
            puts("error: invalid status");
            return 1;
    }

    return 0;
}
SHELL_COMMAND(gpio_enable_int, "enable or disable gpio interrupt", cmd_gpio_enable_int);
#endif

static int cmd_gpio_read(int argc, char **argv)
{
    int port, pin;

    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);
    pin = atoi(argv[2]);

    if (gpio_read(GPIO_PIN(port, pin))) {
        printf("GPIO_PIN(%i.%02i) is HIGH\n", port, pin);
    }
    else {
        printf("GPIO_PIN(%i.%02i) is LOW\n", port, pin);
    }

    return 0;
}
SHELL_COMMAND(gpio_read, "read pin status", cmd_gpio_read);

static int cmd_gpio_set(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_set(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}
SHELL_COMMAND(gpio_set, "set pin to HIGH", cmd_gpio_set);

static int cmd_gpio_clear(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_clear(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}
SHELL_COMMAND(gpio_clear, "set pin to LOW", cmd_gpio_clear);

static int cmd_gpio_toggle(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <port> <pin>\n", argv[0]);
        return 1;
    }

    gpio_toggle(GPIO_PIN(atoi(argv[1]), atoi(argv[2])));

    return 0;
}
SHELL_COMMAND(gpio_toggle, "toggle pin", cmd_gpio_toggle);

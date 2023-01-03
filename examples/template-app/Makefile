APPLICATION = template-app
BOARD ?= lora3a-h10
LORA3ABASE ?= $(CURDIR)/../../
RIOTBASE ?= $(LORA3ABASE)/../RIOT
EXTERNAL_BOARD_DIRS=$(LORA3ABASE)/boards
EXTERNAL_MODULE_DIRS=$(LORA3ABASE)/modules
EXTERNAL_PKG_DIRS=$(LORA3ABASE)/pkg
DEVELHELP ?= 1
QUIET ?= 1
PORT ?= /dev/ttyUSB0

USEMODULE += saml21_cpu_debug
USEMODULE += saml21_backup_mode
USEMODULE += saul_default
USEMODULE += periph_spi_reconfigure
USEMODULE += periph_i2c_reconfigure
USEMODULE += periph_uart_reconfigure

include $(RIOTBASE)/Makefile.include

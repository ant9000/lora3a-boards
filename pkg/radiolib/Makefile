PKG_NAME=radiolib
PKG_URL=https://github.com/jgromes/RadioLib
PKG_VERSION=3fa34b433a528b555998bb1f4d35bf108165a90f # version 5.5.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

#less Wall TODO: get things fixed upstream
CFLAGS += -Wno-return-type
CFLAGS += -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

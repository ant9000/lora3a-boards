PKG_NAME    = soniclib
PKG_VERSION = v2.9.0
PKG_EXT     = zip
PKG_LICENSE = MIT
PKG_URL     = https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2022/06
PKG_SHA512  = d415ea9fa82f4849c2049e9f210f1a975ae5c563bade4e35acff80971f453928832ebf8d4db9a2b62cf5d103a0c060a40886af39895e19fa9c8ea775125c98d0

PKG_ARCHIVE = SmartSonic-CH201-Static-Target-Rejection-Example-$(PKG_VERSION).$(PKG_EXT)
PKG_ARCHIVE_DIR = "SmartSonic CH201 Static Target Rejection Example $(PKG_VERSION)/invn.smartsonic.str-example/source/drivers/chirpmicro"

PKG_BASE_DIR = $(BINDIR)/pkg/$(PKG_NAME)
PKG_SOURCE_DIR = $(PKG_BASE_DIR)/src
PKG_BUILD_DIR = $(PKG_BASE_DIR)/build

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: ..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS = $(INCLUDES)

TOOLCHAIN_FILE = $(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

all: soniclib

prepare:
	@#noop

soniclib: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	cp $(PKG_BUILD_DIR)/libsoniclib_whitney.a $(BINDIR)/libsoniclib_whitney.a

$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE)
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
          -Wno-dev \
          -B$(PKG_BUILD_DIR) \
          -S$(PKG_SOURCE_DIR)

$(TOOLCHAIN_FILE): $(PKG_SOURCE_DIR)/
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

$(PKG_SOURCE_DIR)/: $(PKG_BASE_DIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	$(QQ)test "$(PKG_SHA512)  $<" =  "$$(sha512sum "${<}")"
	cd $(PKG_BASE_DIR) && $(QQ)$(UNZIP_HERE) $<
	$(QQ)ln -s $(PKG_ARCHIVE_DIR) $(PKG_SOURCE_DIR)
	mkdir $(PKG_BUILD_DIR)

$(PKG_BASE_DIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT): $(PKG_BASE_DIR)/
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(PKG_ARCHIVE)

$(PKG_BASE_DIR)/:
	mkdir -p $(PKG_BASE_DIR)

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
        CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
        $(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

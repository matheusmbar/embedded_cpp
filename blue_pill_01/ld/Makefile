OPENCM3_DIR_DEFAULT = $(shell pwd)/../thirdy-party/libopencm3
CPP_DEFAULT = /usr/bin/arm-none-eabi-gcc

ifeq ($(DEVICE),)
$(error 'DEVICE' is not set)
endif

ifeq (,$(wildcard $(CPP)))
$(warning CPP is not valid as '$(CPP)', trying to use $(CPP_DEFAULT))
CPP := $(CPP_DEFAULT)
endif

ifeq (,$(wildcard $(OPENCM3_DIR)))
$(warning '$(OPENCM3_DIR)' does not exist as OPENCM3_DIR, trying to use $(OPENCM3_DIR_DEFAULT))
OPENCM3_DIR := $(OPENCM3_DIR_DEFAULT)
endif

OVERRIDE_LDSCRIPT := $(LDSCRIPT)
ifeq ($(OVERRIDE_LDSCRIPT),)
OVERRIDE_LDSCRIPT = $(DEVICE).ld
endif

include $(OPENCM3_DIR)/mk/genlink-config.mk

LDSCRIPT = $(OVERRIDE_LDSCRIPT)
target = $(subst stm32,stm32/,$(genlink_family))

include $(OPENCM3_DIR)/mk/genlink-rules.mk

.PHONY: clean all arch_flags cppflags libname family subfamily libname

all: $(LDSCRIPT)

arch_flags:
	@echo $(ARCH_FLAGS)

cppflags:
	@echo $(genlink_cppflags)

family:
	@echo $(genlink_family)

subfamily:
	@echo $(genlink_subfamily)

target:
	@echo $(target)

libname:
	@echo $(LIBNAME)

clean:
	$(Q)$(RM) -rf $(LDSCRIPT)

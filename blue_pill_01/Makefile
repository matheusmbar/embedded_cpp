
PROJECT_NAME = blue_pill_01

INCLUDES = -I include


SRC_FILES = src/main.cpp \
 			src/sys/syscalls.c \
			src/sys/startup.c \
			src/sys/sysmem.c \
			src/test_c.c \
			src/test_cpp.cpp


LINKER_FILE = STM32F103C8TX.ld


# Set build software
CROSS_COMPILE  = arm-none-eabi
CC            := $(CROSS_COMPILE)-gcc
CXX           := $(CROSS_COMPILE)-g++


#
# Compile and link flags
#

CPPFLAGS += $(INCLUDES)
CPPFLAGS += -mcpu=cortex-m3 -mthumb -msoft-float
CPPFLAGS += -g

# Use newlib nano, optimized to embedded
CPPFLAGS += --specs=nosys.specs
CPPFLAGS += --specs=nano.specs

# Disable exceptions
CXXFLAGS += -fno-exceptions

LDFLAGS  += -T $(LINKER_FILE)

# Remove unused code
CPPFLAGS += -ffunction-sections -fdata-sections
LDFLAGS  += -Wl,--gc-sections


#
# Set build files
#

BUILDDIR = build

# Create Object files (.o) list from SRC_FILES list
OBJ_FILES  :=$(SRC_FILES:.c=.o)
OBJ_FILES  :=$(OBJ_FILES:.cpp=.o)
OBJ_FILES  := $(addprefix $(BUILDDIR)/, $(OBJ_FILES))


# Binary filenames
ELF_FILENAME := $(BUILDDIR)/$(PROJECT_NAME).elf
BIN_FILENAME := $(BUILDDIR)/$(PROJECT_NAME).bin


#
# Build rules
#

# Compile C files
$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

# Compile CPP files
$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

# Link and create final binary
$(ELF_FILENAME): $(OBJ_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(OBJ_FILES) -o $@
	$(CROSS_COMPILE)-objcopy -O binary $(ELF_FILENAME) $(BIN_FILENAME)
	$(CROSS_COMPILE)-size $(ELF_FILENAME)


clean:
	$(RM) $(OBJ_FILES) src/main
	$(RM) $(ELF_FILENAME) $(BIN_FILENAME)

echo:
	@echo LDFLAGS:  $(LDFLAGS)
	@echo CFLAGS:   $(CFLAGS)
	@echo CPPFLAGS: $(CPPFLAGS)
	@echo CXXFLAGS: $(CXXFLAGS)
	@echo ""
	@echo OBJ_FILES:  $(OBJ_FILES)
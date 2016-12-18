EXEC := isolated-can-repeater

PREFIX := msp430-elf-

CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
LD := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump
SIZE := $(PREFIX)size

CPPFLAGS := -Os -Wall -Wextra -g -nostdlib -mmcu=msp430g2553
CFLAGS := -std=c11 $(CPPFLAGS)
CXXFLAGS := -std=c++1y -fno-exceptions -fno-rtti -flto $(CPPFLAGS)
LDFLAGS := -Wl,-Os -mmcu=msp430g2553

C_SRC := $(wildcard *.c)
CXX_SRC := $(wildcard *.cpp)

OBJDIR := build

C_OBJ := $(addprefix $(OBJDIR)/,$(C_SRC:.c=.o))
CXX_OBJ := $(addprefix $(OBJDIR)/,$(CXX_SRC:.cpp=.o))

.PHONY: all
all: $(OBJDIR)/$(EXEC).hex $(OBJDIR)/$(EXEC).lst

$(OBJDIR)/$(EXEC).elf: $(C_OBJ) $(CXX_OBJ)
	@mkdir -p $(@D)
	@echo "[LD] $@"
	@$(LD) $(LDFLAGS) -o "$@" $(C_OBJ) $(CXX_OBJ)
	@$(SIZE) "$@"

%.hex: %.elf
	@mkdir -p $(@D)
	@echo "[HEX] $@"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

%.lst: %.elf
	@mkdir -p $(@D)
	@echo "[LST] $@"
	@$(OBJDUMP) -DS $< > $@

$(C_OBJ): $(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(CXX_OBJ): $(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "[CXX] $@"
	@$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

.PHONY: clean
clean:
	-$(RM) -r $(OBJDIR)

.PHONY: upload
upload: $(OBJDIR)/$(EXEC).elf
	mspdebug -q rf2500 "prog $(OBJDIR)/$(EXEC).elf"

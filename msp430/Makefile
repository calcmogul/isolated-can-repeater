EXEC := isolated-can-repeater

PREFIX := msp430-elf-

CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
LD := $(PREFIX)gcc
SIZE := $(PREFIX)size

CPPFLAGS := -O3 -Wall -Wextra -flto -nostdlib -mmcu=msp430g2553
CFLAGS := -std=c11 $(CPPFLAGS)
CXXFLAGS := -std=c++1y -fno-exceptions -fno-rtti $(CPPFLAGS)
LDFLAGS := -O3 -flto -mmcu=msp430g2553

C_SRC := $(wildcard *.c)
CXX_SRC := $(wildcard *.cpp)

OBJDIR := build

C_OBJ := $(addprefix $(OBJDIR)/,$(C_SRC:.c=.o))
CXX_OBJ := $(addprefix $(OBJDIR)/,$(CXX_SRC:.cpp=.o))

.PHONY: all
all: $(OBJDIR)/$(EXEC).out

$(OBJDIR)/$(EXEC).out: $(C_OBJ) $(CXX_OBJ)
	@mkdir -p $(@D)
	@echo "[LD] $@"
	@$(LD) $(LDFLAGS) -o "$@" $(C_OBJ) $(CXX_OBJ)
	@$(SIZE) "$@"

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
upload: all
	mspdebug -q rf2500 "prog $(OBJDIR)/$(EXEC).out"

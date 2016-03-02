# ------------------------------------------------
# Generic Makefile
#
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = Receiver
MCU = atmega128rfa1
CC       = $(AVR_HOME)/bin/avr-gcc
# compiling flags here
CFLAGS   = -mmcu=$(MCU) -std=c99 -Wall -I. -Werror

LINKER   = $(AVR_HOME)/bin/avr-gcc -o
OBJCOPY   = $(AVR_HOME)/bin/avr-objcopy
# linking flags here
LDFLAGS = -mmcu=$(MCU)  -Wall -g  -Werror 

# change these to set the proper directories where each files shoould be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -rf


.PHONEY : help
help:
	@echo "$(TARGET) help:"
	@echo "		all: Compiles and builds (default), runs cppcheck and doxygen"
	@echo "		clean: cleans obj dir"
	@echo "		remove: cleans obj and bin dir"
	@echo "		cppcheck: runs cppcheck"
	@echo "		doxygen: runs doxygen"

.PHONEY all : cppcheck $(BINDIR)/$(TARGET) doxygen

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@.elf  $(OBJECTS) -lm
	$(OBJCOPY) -O binary -R .eeprom -R .nwram  $@.elf $@.bin
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram  $@.elf $@.hex
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(BINDIR):
	@mkdir $(BINDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

.PHONEY: clean
clean:
	@$(rm) $(OBJDIR)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)
	@echo "Executable removed!"

.PHONEY : cppcheck
cppcheck:
	@echo "Running cppcheck..."
	@cppcheck   -q  --enable=all --std=c99   $(SRCDIR)/*.c  -I $(AVR_HOME)/avr/include -D__AVR_ATmega128RFA1__ 
	@echo "Done!"

.PHONEY : doxygen
doxygen:
	@echo "Running doxygen..."
	@doxygen
	@echo "Done!"

# = = Configurable parameters
PROGRAMMER	?= arduino_as_isp
PORT		?= /dev/cu.usbmodem11301
BAUD		?= 19200
F_CPU		?= 1000000UL

# = = Compiler-specific info
CC			= avr-gcc
OBJCOPY		= avr-objcopy
AVRDUDE		= avrdude
CFLAGS		= -DF_CPU=$(F_CPU) -Os

# = = Targets
TARGETS	= tx rx
MCU_tx	= atmega328p
MCU_rx	= attiny25
DUDE_MCU_tx	= m328p
DUDE_MCU_rx	= t25

# Sources
SRC_tx = tx.c
SRC_rx = rx.c

# Outputs
ELF_tx = tx.elf
ELF_rx = rx.elf
HEX_tx = tx.hex
HEX_rx = rx.hex

# = = Build Rules
all: $(TARGETS)
tx: $(HEX_tx)
rx: $(HEX_rx)

%.elf:
	$(CC) -mmcu=$(MCU_$*) $(CFLAGS) -o $@ $(SRC_$*)

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# = = Flash Rules
flash-tx: tx
	$(AVRDUDE) -p $(DUDE_MCU_tx) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) \
		-U flash:w:$(HEX_tx):i -F

flash-rx: rx
	$(AVRDUDE) -p $(DUDE_MCU_rx) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) \
		-U flash:w:$(HEX_rx):i -F

# = = Utility
ping-tx:
	$(AVRDUDE) -p $(DUDE_MCU_tx) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -v

ping-rx:
	$(AVRDUDE) -p $(DUDE_MCU_rx) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -v

clean:
	rm -rf *.elf *.hex

.PHONY: all tx rx flash-tx flash-rx ping-tx ping-rx clean

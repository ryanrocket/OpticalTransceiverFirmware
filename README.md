# Firmware for Optical Transceiver 
Senior design project by Ryan Wans and Alek Taranov, 2026.

## What to do
* `make ping-tx` and `make ping-rx` confirm that the respective MCUs are connected through the ISP and visible through the USB interface.
* `make tx` and `make rx` compile the `tx.c` and `rx.c` into hex files, respectively.
* `make flash-tx` and `make flash-rx` flash the TX and RX MCUs, respectively.
* `make clean` to remove HEX and ELF files.

## Flashing
0. Modify the Makefile as needed to add the correct USB port to flash via
1. Run `make clean`
2. Run `make rx` or `make tx`
3. Run `make flash-rx` or `make flash-tx`

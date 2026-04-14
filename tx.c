#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

#define CSEL_PIN PD5
#define SCK_PIN  PB5 
#define SDA_PIN  PC4
#define LED_PIN  PD7

// MAX3798 Register Addresses
#define ADDR_RXCTRL1    0x00
#define ADDR_RXCTRL2    0x01
#define ADDR_SET_CML    0x03
#define ADDR_TXCTRL     0x05
#define ADDR_SET_IBIAS  0x08
#define ADDR_SET_IMOD   0x09
#define ADDR_MODECTRL   0x0E

// Function prototypes
static void max3798_write(uint8_t address, uint8_t data);
static uint8_t max3798_read(uint8_t address);
static bool max3798_verify_register(uint8_t address, uint8_t expected);

int main(void) {
    // = = = = Set all pins as output = = = =
    DDRD |= (1 << LED_PIN);
    DDRB |= (1 << SCK_PIN);
    DDRC |= (1 << SDA_PIN);
    DDRD |= (1 << CSEL_PIN);

    // Idle states for bit-banged serial interface
    PORTB &= ~(1 << SCK_PIN);
    PORTC &= ~(1 << SDA_PIN);
    PORTD |= (1 << CSEL_PIN); // deassert CSEL (active-low)

    // Single-register sanity test:
    // write MODECTRL, then read back MODECTRL.
    // max3798_write(ADDR_MODECTRL, 0x12);
    // _delay_ms(1);
    // bool match = max3798_verify_register(ADDR_MODECTRL, 0x12);
    PORTD &= ~(1 << LED_PIN);

    while (1) {
        PORTD &= ~(1 << LED_PIN);
        _delay_ms(500);
        PORTD |= (1 << LED_PIN);
        _delay_ms(500);
    }
}

static void max3798_write(uint8_t address, uint8_t data) {
    uint16_t frame = ((uint16_t)address << 8) | data;

    PORTD &= ~(1 << CSEL_PIN);   // assert CSEL (active-low)
    _delay_us(1);

    for (int i = 15; i >= 0; i--) {
        PORTB &= ~(1 << SCK_PIN); // data valid before rising edge

        if (frame & (1 << i))
            PORTC |= (1 << SDA_PIN);
        else
            PORTC &= ~(1 << SDA_PIN);

        _delay_us(1);

        PORTB |= (1 << SCK_PIN);
        _delay_us(1);
    }

    PORTB &= ~(1 << SCK_PIN);
    PORTC &= ~(1 << SDA_PIN);
    _delay_us(1);
    PORTD |= (1 << CSEL_PIN);    // deassert CSEL
    _delay_us(1);
}

static uint8_t max3798_read(uint8_t address) {
    uint8_t data = 0;
    uint8_t read_cmd = (uint8_t)(0x80 | (address & 0x7F));

    // Send read command
    PORTD &= ~(1 << CSEL_PIN);   // assert CSEL
    _delay_us(1);

    for (int i = 7; i >= 0; i--) {
        PORTB &= ~(1 << SCK_PIN);

        if (read_cmd & (1 << i))
            PORTC |= (1 << SDA_PIN);
        else
            PORTC &= ~(1 << SDA_PIN);

        _delay_us(1);
        PORTB |= (1 << SCK_PIN);
        _delay_us(1);
    }

    // Turn SDA into input to read returning data byte
    DDRC &= ~(1 << SDA_PIN);
    PORTC &= ~(1 << SDA_PIN); // no pull-up

    for (int i = 7; i >= 0; i--) {
        PORTB &= ~(1 << SCK_PIN);
        _delay_us(1);

        PORTB |= (1 << SCK_PIN);
        _delay_us(1);

        if (PINC & (1 << SDA_PIN))
            data |= (1 << i);
    }

    PORTB &= ~(1 << SCK_PIN);
    _delay_us(1);
    PORTD |= (1 << CSEL_PIN);    // deassert CSEL
    _delay_us(1);

    // Restore SDA as output for next writes
    DDRC |= (1 << SDA_PIN);
    PORTC &= ~(1 << SDA_PIN);

    return data;
}

static bool max3798_verify_register(uint8_t address, uint8_t expected) {
    uint8_t readback = max3798_read(address);
    return (readback == expected);
}
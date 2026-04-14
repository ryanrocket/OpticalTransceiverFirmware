#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>

#include "tx_diag.h"

#define LED_PIN PD7

typedef struct {
    uint8_t address;
    uint8_t expected;
} reg_expect_t;

// Important TX-path related setup registers from the init sequence
static const reg_expect_t k_regs[] = {
    {0x0E, 0x12}, // MODECTRL
    {0x01, 0x75}, // RXCTRL2
    {0x03, 0x80}, // SET_CML
    {0x05, 0x01}, // TXCTRL
    {0x08, 0x40}, // SET_IBIAS
    {0x09, 0x40}, // SET_IMOD
};

static void led_on(void) {
    PORTD |= (1 << LED_PIN);
}

static void led_off(void) {
    PORTD &= ~(1 << LED_PIN);
}

static void led_pulse(uint16_t on_ms, uint16_t off_ms) {
    led_on();
    while (on_ms--) {
        _delay_ms(1);
    }

    led_off();
    while (off_ms--) {
        _delay_ms(1);
    }
}

static void led_show_register_index(uint8_t index) {
    // Register marker: (index + 1) quick pulses
    uint8_t pulses = (uint8_t)(index + 1);

    for (uint8_t i = 0; i < pulses; i++) {
        led_pulse(80, 120);
    }

    _delay_ms(250);
}

static void led_show_byte_bits(uint8_t value) {
    // 8-bit state marker: long ON = 1, short ON = 0 (MSB first)
    for (int8_t bit = 7; bit >= 0; bit--) {
        if (value & (1U << bit)) {
            led_pulse(220, 120);
        } else {
            led_pulse(70, 120);
        }
    }

    _delay_ms(250);
}

static void led_show_pass_fail(bool is_ok) {
    if (is_ok) {
        // PASS marker: one long pulse
        led_pulse(500, 250);
    } else {
        // FAIL marker: three quick pulses
        led_pulse(90, 90);
        led_pulse(90, 90);
        led_pulse(90, 250);
    }
}

static void led_show_next_register_marker(void) {
    // Transition marker: two short pulses
    led_pulse(60, 90);
    led_pulse(60, 220);
}

void tx_led_register_walk(max3798_read_fn_t read_fn) {
    if (read_fn == 0) {
        // Obvious error pattern if callback is missing
        while (1) {
            led_pulse(50, 50);
        }
    }

    while (1) {
        for (uint8_t i = 0; i < (uint8_t)(sizeof(k_regs) / sizeof(k_regs[0])); i++) {
            uint8_t reg_value;
            bool is_ok;

            led_show_register_index(i);

            reg_value = read_fn(k_regs[i].address);
            is_ok = (reg_value == k_regs[i].expected);

            // State of register (actual readback bits)
            led_show_byte_bits(reg_value);

            // Match/mismatch against expected value
            led_show_pass_fail(is_ok);

            // Indicate moving to next register
            led_show_next_register_marker();
        }

        // End-of-list marker
        _delay_ms(600);
        led_pulse(200, 120);
        led_pulse(200, 120);
        _delay_ms(900);
    }
}
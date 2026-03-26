#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set PB3 as output
    DDRB |= (1 << PB3);

    while (1) {
        // Toggle LED on PB3
        PORTB ^= (1 << PB3);

        // 500 ms delay
        _delay_ms(500);
    }
}

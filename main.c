#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	// Set PD7 as output
	DDRD |= (1 << PD7);

	while (1) {
		// Toggle LED at PD7
		PORTD ^= (1 << PD7);

		// period 500ms
		_delay_ms(500);
	}
}

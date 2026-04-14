#ifndef TX_DIAG_H
#define TX_DIAG_H

#include <stdint.h>

typedef uint8_t (*max3798_read_fn_t)(uint8_t address);

void tx_led_register_walk(max3798_read_fn_t read_fn);

#endif
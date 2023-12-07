#ifndef TWI_H_
#define TWI_H_

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>
#include "../../defines.h"

void twi_init(uint32_t frequency);
int8_t twi_write(uint8_t address, uint8_t *data, uint8_t length);
int8_t twi_read(uint8_t address, uint8_t *data, uint8_t length);

#endif // TWI_H_
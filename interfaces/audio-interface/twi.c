#include "twi.h"

const uint16_t twi_prescaler_values[] = {0, 1, 4, 16, 64};

void twi_start() {
    // Send start signal
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // Wait for TWINT flag to be set
    while (!(TWCR & (1 << TWINT)));
}

//send stop signal
void twi_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void twi_write_single(uint8_t u8data)
{
    TWDR = u8data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
}

uint8_t twi_read_single(bool ack)
{
    if (ack)
    {
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
        while ((TWCR & (1<<TWINT)) == 0);
        return TWDR;
    }
    else
    {
        TWCR = (1<<TWINT)|(1<<TWEN);
        while ((TWCR & (1<<TWINT)) == 0);
        return TWDR;
    }
}

uint8_t twi_get_status(void)
{
    uint8_t status;
    //mask status
    status = TWSR & 0xF8;
    return status;
}

int8_t twi_write(uint8_t addr, uint8_t *data, uint8_t len) {
    // Send start signal
    twi_start();

    // Check for error
    if (twi_get_status() != TW_START)
    {
        return -1;
    }
    // Send device address with write bit
    twi_write_single(addr << 1);
    // Check for error
    if (twi_get_status() != TW_MT_SLA_ACK)
    {
        return -1;
    }
    // che
    // Send data
    for (uint8_t i = 0; i < len; i++)
    {
        twi_write_single(data[i]);
        // Check for error
        if (twi_get_status() != TW_MT_DATA_ACK)
        {
            return -1;
        }
    }
    // Send stop signal
    twi_stop();
    return 0;
}

int8_t twi_read(uint8_t addr, uint8_t *data, uint8_t len) {
    // Send start signal
    twi_start();
    // Check for error
    if (twi_get_status() != TW_START)
    {
        return -1;
    }
    // Send device address with read bit
    twi_write_single((addr << 1) | 1);
    // Check for error
    if (twi_get_status() != TW_MR_SLA_ACK)
    {
        return -1;
    }
    // Read data
    for (uint8_t i = 0; i < len; i++)
    {
        data[i] = twi_read_single(i < len - 1);
        // Check for error
        if (twi_get_status() != TW_MR_DATA_ACK)
        {
            return -1;
        }
    }
    // Send stop signal
    twi_stop();
    return 0;
}

void twi_init(uint32_t frequency) {
    // Initialize the twi_device struct
    uint8_t prescaler = 0;
    uint16_t bitrate_register = 0;
    // Find the prescaler value that gives SCL = 100 kHz
    for (prescaler = 0; prescaler < 5; prescaler++)
    {
        bitrate_register = (F_CPU / frequency - 16) / (2 * twi_prescaler_values[prescaler]);
        if (bitrate_register <= 255)
        {
            break;
        }
    }

    // Set bitrate prescaler
    TWSR = (0b11 & prescaler);
    // Set bitrate register
    TWBR = bitrate_register;

    // Enable TWI
    TWCR = (1 << TWEN);
}
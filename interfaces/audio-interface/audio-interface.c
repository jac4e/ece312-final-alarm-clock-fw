#include "audio-interface.h"
#include <stddef.h>

// void audio_send_command(struct audio_device *dev, audio_command command, uint8_t data) {
//     // Send command to audio device
//     TWI_start();
//     TWI_write(AUDIO_DEVICE_ADDR);
//     TWI_write(command);
//     TWI_write(data);
//     TWI_stop();
// }

void audio_mute_basic(struct audio_device *dev) {
    // Disconnect OC0A (aka AUDIO_PWM_AMP_SHDN_PORT) from timer0
    TCCR0A &= ~(1 << COM0A0);
    dev->_is_muted = true;
}

void audio_unmute_basic(struct audio_device *dev) {
    // Connect OC0A (aka AUDIO_PWM_AMP_SHDN_PORT) to timer0
    TCCR0A |= (1 << COM0A0);
    dev->_is_muted = false;
}

void audio_set_volume_basic(struct audio_device *dev, int volume) {
    // Not implemented with basic audio device
    // unsure how to implement over PWM
}

const uint16_t timer0_prescaler_values[] = {0, 1, 8, 64, 256, 1024};

void audio_set_frequency(struct audio_device *dev, uint16_t freq) {
    for (dev->_prescale = 0; dev->_prescale < 6; dev->_prescale++)
    {
        dev->_top = EXT_CLK / (2 * freq * (uint64_t)timer0_prescaler_values[dev->_prescale]) - 1;
        if (dev->_top <= 255)
        {
            break;
        }
    }
    OCR0A = dev->_top;
}

// void audio_mute_premium(struct audio_device *dev) {
//     // Set AUDIO_PWM_AMP_SHDN_PORT pin to low
//     PORTC &= ~(1 << AUDIO_PWM_AMP_SHDN_PORT);
// }

// void audio_unmute_premium(struct audio_device *dev) {
//     // Set AUDIO_PWM_AMP_SHDN_PORT pin to high
//     PORTC |= (1 << AUDIO_PWM_AMP_SHDN_PORT);
// }

// void audio_set_volume_premium(struct audio_device *dev, int volume) {
//     // Send volume command to audio device
//     audio_send_command(dev, AUDIO_SET_VOLUME, volume);
// }

// Function to initialize the audio device
void audio_interface_init(struct audio_device *dev) {
    // Initialize ID Port (AUDIO_ID_PORT=PD6) as input
    DDRD &= ~(1 << AUDIO_ID_PORT);

    // Set ID
    // dev->_id = (PIND & (1 << AUDIO_ID_PORT)) >> AUDIO_ID_PORT;
    dev->_id = AUDIO_BASIC;
    
    // Initialize device parameters
    dev->_is_muted = true;
    dev->_volume = 0;
    dev->_prescale = 0;
    dev->_top = 0;

    // Initialize function pointers
    if (dev->_id == AUDIO_BASIC) {
        dev->mute = audio_mute_basic;
        dev->unmute = audio_unmute_basic;
        dev->set_volume = audio_set_volume_basic;
        dev->send_command = NULL;
    } else if (dev->_id == AUDIO_PREMIUM) {
        // dev->mute = audio_mute_premium;
        // dev->unmute = audio_unmute_premium;
        // dev->set_volume = audio_set_volume_premium;
        // dev->send_command = audio_send_command;
        dev->send_command = NULL;
    }

    if (dev->_id == AUDIO_BASIC) {
        // Set AUDIO_PWM_AMP_SHDN_PORT pin as output
        DDRD |= (1 << AUDIO_PWM_AMP_SHDN_PORT);
        // Set AUDIO_PWM_AMP_SHDN_PORT pin to low
        PORTD &= ~(1 << AUDIO_PWM_AMP_SHDN_PORT);

        // Initialize timer0 forCTC Mode with OC0A (aka AUDIO_PWM_AMP_SHDN_PORT) disconnected
        TCCR0A |= (1 << WGM01);
        TCCR0B |= (0b111 & dev->_prescale);

        audio_set_frequency(dev, 420);
        
        audio_unmute_basic(dev);
    } else {
        // Initialize TWI
        // TWI_init();
        // Set PWM pin as output
    }
}
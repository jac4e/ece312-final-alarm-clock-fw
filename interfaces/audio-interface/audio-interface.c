#include "audio-interface.h"
#include "twi.h"
#include "../../common.h"
#include <stddef.h>

void audio_send_command(audio_device_t *dev, audio_command command, uint8_t data) {
    switch (command)
    {
    case AUDIO_PLAY:
        PORTC |= (1 << AUDIO_PLAYPAUSE_PORT);
        break;
    case AUDIO_PAUSE:
        PORTC &= ~(1 << AUDIO_PLAYPAUSE_PORT);
    case AUDIO_CHANGE_TRACK:
        // Set sel0 and sel1 based on data

        switch (data)
        {
        case 0:
            PORTD &= ~(1 << AUDIO_PWM_SEL0_PORT);
            PORTC &= ~(1 << AUDIO_SEL1_PORT);
            break;
        case 1:
            PORTD |= (1 << AUDIO_PWM_SEL0_PORT);
            PORTC &= ~(1 << AUDIO_SEL1_PORT);
            break;
        case 2:
            PORTD &= ~(1 << AUDIO_PWM_SEL0_PORT);
            PORTC |= (1 << AUDIO_SEL1_PORT);
            break;
        case 3:
            PORTD |= (1 << AUDIO_PWM_SEL0_PORT);
            PORTC |= (1 << AUDIO_SEL1_PORT);
            break;
        default:
            break;
        }
    }
}

void audio_mute_basic(audio_device_t *dev) {
    // Disconnect OC0A (aka AUDIO_PWM_SEL0_PORT) from timer0
    TCCR0A &= ~(1 << COM0A0);
    dev->_is_muted = true;
}

void audio_unmute_basic(audio_device_t *dev) {
    // Connect OC0A (aka AUDIO_PWM_SEL0_PORT) to timer0
    TCCR0A |= (1 << COM0A0);
    dev->_is_muted = false;
}

void audio_set_volume_basic(audio_device_t *dev, int volume) {
    // Not implemented with basic audio device
    // unsure how to implement over PWM
}

const uint16_t timer0_prescaler_values[] = {0, 1, 8, 64, 256, 1024};

void audio_set_frequency(audio_device_t *dev, uint16_t freq) {
    for (dev->_prescale = 0; dev->_prescale < 6; dev->_prescale++)
    {
        dev->_top = F_CPU / (2 * freq * (uint64_t)timer0_prescaler_values[dev->_prescale]) - 1;
        if (dev->_top <= 255)
        {
            break;
        }
    }
    TCCR0B |= (0b111 & dev->_prescale);
    OCR0A = dev->_top;
    dev->_freq = freq;
}

void audio_mute_premium(audio_device_t *dev) {
    audio_send_command(dev, AUDIO_PAUSE, 0);
}

void audio_unmute_premium(audio_device_t *dev) {
    audio_send_command(dev, AUDIO_PLAY, 0);
}

void audio_set_volume_premium(audio_device_t *dev, int volume) {
    // Send volume command to audio device
    audio_send_command(dev, AUDIO_SET_VOLUME, volume);
}

// Function to initialize the audio device
void audio_interface_init(audio_device_t *dev) {
    // Initialize ID Port (AUDIO_ID_PORT=PD6) as input
    DDRD &= ~(1 << AUDIO_ID_PORT);

    // Set ID
    dev->_id = (PINC & (1 << AUDIO_ID_PORT)) >> AUDIO_ID_PORT;
    
    // Initialize device parameters
    dev->_is_muted = true;
    dev->_volume = 0;
    dev->_prescale = 0;
    dev->_top = 0;
    dev->_freq = 0;

    // Initialize function pointers
    if (dev->_id == AUDIO_BASIC) {
        dev->mute = audio_mute_basic;
        dev->unmute = audio_unmute_basic;
        dev->set_volume = audio_set_volume_basic;
        dev->set_freq = audio_set_frequency;
        dev->send_command = common_nop;
    } else if (dev->_id == AUDIO_PREMIUM) {
        dev->mute = audio_mute_premium;
        dev->unmute = audio_unmute_premium;
        dev->set_volume = common_nop;
        dev->send_command = audio_send_command;
        dev->set_freq = common_nop;
    }

    if (dev->_id == AUDIO_BASIC) {
        // Set AUDIO_PWM_SEL0_PORT pin as output
        DDRD |= (1 << AUDIO_PWM_SEL0_PORT);
        // Set AUDIO_PWM_SEL0_PORT pin to low
        PORTD &= ~(1 << AUDIO_PWM_SEL0_PORT);

        // Initialize timer0 forCTC Mode with OC0A (aka AUDIO_PWM_SEL0_PORT) disconnected
        TCCR0A = (1 << WGM01);
        
        // Set frequency
        audio_set_frequency(dev, 220);
    } else {
        // Initialize AUDIO_PWM_SEL0_PORT pin as output for sel0
        DDRD |= (1 << AUDIO_PWM_SEL0_PORT);
        // Initialize AUDIO_SEL1_PORT pin as output for sel1
        DDRC |= (1 << AUDIO_SEL1_PORT);
        // Initialize AUDIO_PLAYPAUSE_PORT pin as output for play/pause
        DDRC |= (1 << AUDIO_PLAYPAUSE_PORT);
    }
}
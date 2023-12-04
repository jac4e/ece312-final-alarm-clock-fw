#ifndef audio_INTERFACE_H
#define audio_INTERFACE_H

// Include any necessary headers here
#include "../../defines.h"
#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    AUDIO_BASIC,
    AUDIO_PREMIUM
} audio_id;

typedef enum {
    AUDIO_SET_VOLUME,
    AUDIO_PLAY,
    AUDIO_PAUSE,
    AUDIO_CHANGE_TRACK,
} audio_command;

/**
 * @brief Audio Device Interface Structure
 * 
 * This structure defines the interface for the audio device. It contains
 * function pointers for the device operations as well as any parameters.
 * 
 */
struct audio_device {
    // Add any device parameters here
    uint8_t _prescale; // only used for basic audio devices
    uint8_t _top; // only used for basic audio devices
    uint16_t _freq; // only used for basic audio devices
    audio_id _id; // 1 is basic, 0 is premium
    bool _is_muted;
    uint8_t _volume;

    // Function pointers for device operations
    void (*mute)(struct audio_device *dev);
    void (*unmute)(struct audio_device *dev);
    void (*set_volume)(struct audio_device *dev, int volume);
    void (*send_command)(struct audio_device *dev, audio_command command, uint8_t data); // Only used for premium audio devices
    void (*set_freq)(struct audio_device *dev, uint16_t freq); // Only used for basic audio devices
    // Add more function pointers for other operations
};



#endif // audio_INTERFACE_H
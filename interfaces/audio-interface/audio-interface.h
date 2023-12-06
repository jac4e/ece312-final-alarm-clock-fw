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

// Forward declaration of the audio device structure
typedef volatile struct audio_device_t audio_device_t;

/**
 * @brief Audio Device Interface Structure
 * 
 * This structure defines the interface for the audio device. It contains
 * function pointers for the device operations as well as any parameters.
 * 
 */
struct audio_device_t {
    // Add any device parameters here
    uint8_t _prescale; // only used for basic audio devices
    uint16_t _top; // only used for basic audio devices
    uint16_t _freq; // only used for basic audio devices
    audio_id _id; // 1 is basic, 0 is premium
    bool _is_muted;
    uint8_t _volume;

    // Function pointers for device operations
    void (*mute)(audio_device_t *dev);
    void (*unmute)(audio_device_t *dev);
    void (*set_volume)(audio_device_t *dev, int volume);
    void (*send_command)(audio_device_t *dev, audio_command command, uint8_t data); // Only used for premium audio devices
    void (*set_freq)(audio_device_t *dev, uint16_t freq); // Only used for basic audio devices
    // Add more function pointers for other operations
};


// Function to initialize the audio device
void audio_interface_init(audio_device_t *dev);



#endif // audio_INTERFACE_H
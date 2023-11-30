#ifndef audio_INTERFACE_H
#define audio_INTERFACE_H

// Include any necessary headers here
#include <avr/io.h>
#include <util/twi.h>

/**
 * @brief Audio Device Interface Structure
 * 
 * This structure defines the interface for the audio device. It contains
 * function pointers for the device operations as well as any parameters.
 * 
 */
struct audio_device {
    // Add any device parameters here
    int audio_param1;
    // ...

    // Function pointers for device operations
    void (*audio_read)(struct audio_device *dev);
    void (*audio_write)(struct audio_device *dev, uint8_t data);
    // Add more function pointers for other operations
};



#endif // audio_INTERFACE_H

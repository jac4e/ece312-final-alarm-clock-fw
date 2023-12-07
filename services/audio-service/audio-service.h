#ifndef audio_SERVICE_H
#define audio_SERVICE_H

#include "../../defines.h"
#include <stdbool.h>
#include <stdint.h>
#include "../../interfaces/audio-interface/audio-interface.h"

// Enums

// alarm type enum
typedef enum {
    AUDIO_SERVICE_ALARM_1, // Basic: 220 Hz, Premium: Alarm 1
    AUDIO_SERVICE_ALARM_2, // Basic: 440 Hz, Premium: Alarm 2
    AUDIO_SERVICE_ALARM_3, // Basic: 880 Hz, Premium: Alarm 3
    AUDIO_SERVICE_ALARM_4, // Basic: 1760 Hz, Premium: Alarm 4
} audio_service_alarm_type;

// Forward declaration of the audio service structure
typedef volatile struct audio_service audio_service;

// Define the structure for the audio service
struct audio_service {
    // Add any service parameters here
    audio_device *_audio_interface;
    audio_service_alarm_type _alarm_type;
    bool _is_playing;
    uint32_t _alarm_counter;

    // Function pointers for service operations
    void (*update)(audio_service *service);
    void (*play)(audio_service *service);
    void (*pause)(audio_service *service);
    void (*change_alarm)(audio_service *service, audio_service_alarm_type alarm);
};

// Define the init function for the audio service
void audio_service__init(audio_service *service, audio_device *audio_interface);

#endif // audio_SERVICE_H

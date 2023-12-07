#include "audio-service.h"

// Function to update the audio service
// Responsible for toggling the audio output for basic audio devices so that it beeps not just has a continuous tone
void audio_service_update(audio_service *service) {
    if (service->_is_playing == false || service->_audio_interface->_id == AUDIO_PREMIUM)
    {
        return;
    }

    // We need to toggle the mute state every 0.5 seconds to get a beep
    // This is only used for basic audio devices
    // Basically toggle audio device mute after 0.5 seconds (once _alarm_counter reaches _audio_interface->_frequency / 2)
    if (++service->_alarm_counter < (service->_audio_interface->_freq/2))
    {
        return;
    }
    else
    {
        if (service->_audio_interface->_is_muted)
        {
            service->_audio_interface->unmute(service->_audio_interface);
        }
        else
        {
            service->_audio_interface->mute(service->_audio_interface);
        }
        service->_alarm_counter = 0;
    }
}

// Function to play the alarm
void audio_service_play(audio_service *service) {
    if (service->_audio_interface->_id == AUDIO_BASIC) {
        // Enable the overflow interrupt for Timer0
        TIMSK0 |= (1 << OCIE0A);
    };

    // Set the is playing flag
    service->_is_playing = true;

    // unmute the audio device
    service->_audio_interface->unmute(service->_audio_interface);
}

// Function to pause the alarm
void audio_service_pause(audio_service *service) {
    if (service->_audio_interface->_id == AUDIO_BASIC) {
        // Disable the overflow interrupt for Timer0
        TIMSK0 &= ~(1 << OCIE0A);
    };

    // Clear the is playing flag
    service->_is_playing = false;

    // mute the audio device
    service->_audio_interface->mute(service->_audio_interface);
}

// Function to change the alarm type
void audio_service_change_alarm(audio_service *service, audio_service_alarm_type alarm_type) {
    service->_alarm_type = alarm_type;

    if (service->_audio_interface->_id == AUDIO_BASIC) {
        switch (alarm_type)
        {
        case AUDIO_SERVICE_ALARM_1:
            service->_audio_interface->set_freq(service->_audio_interface, 220);
            break;

        case AUDIO_SERVICE_ALARM_2:
            service->_audio_interface->set_freq(service->_audio_interface, 330);
            break;

        case AUDIO_SERVICE_ALARM_3:
            service->_audio_interface->set_freq(service->_audio_interface, 400);
            break;

        case AUDIO_SERVICE_ALARM_4:
            service->_audio_interface->set_freq(service->_audio_interface, 440);
            break;
        
        default:
            break;
        }
    };
}


// Function to initialize the audio service
void audio_service_init(audio_service *service, audio_device *audio_interface) {
    // Initialize service parameters
    service->_audio_interface = audio_interface;
    service->_is_playing = false;
    service->_alarm_type = AUDIO_SERVICE_ALARM_1;
    service->_alarm_counter = 0;

    // Initialize function pointers
    service->update = audio_service_update;
    service->play = audio_service_play;
    service->pause = audio_service_pause;
    service->change_alarm = audio_service_change_alarm;

    if (service->_audio_interface->_id == AUDIO_BASIC) {
        // Enable the overflow interrupt for Timer0
        TIMSK0 |= (1 << OCIE0A);
    } else if (service->_audio_interface->_id == AUDIO_PREMIUM) {
        // Disable the overflow interrupt for Timer0
        TIMSK0 &= ~(1 << OCIE0A);
    }

    // Set alarm to alarm 1 (the default)
    service->change_alarm(service, AUDIO_SERVICE_ALARM_1);

}
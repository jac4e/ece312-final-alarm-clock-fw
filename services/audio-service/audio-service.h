#ifndef audio_SERVICE_H
#define audio_SERVICE_H


// Define the structure for the audio service
struct audio_service {
    // Add any service parameters here
    int audio_param1;
    // ...

    // Function pointers for service operations
    // Function for updating the service
    void (*audio_update)(struct audio_service *service); // Can be one single update function, or many functions for different operations during different interupts or timers.
    // Function that sleeps the service
    void (*audio_sleep)(struct audio_service *service);
    // Function that wakes the service
    void (*audio_wake)(struct audio_service *service);
    // Add more function pointers for other operations
};

#endif // audio_SERVICE_H
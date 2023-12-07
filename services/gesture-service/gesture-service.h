#ifndef GESTURE_SERVICE_H
#define GESTURE_SERVICE_H

#include <time.h>
#include <stdbool.h>
#include "../clock-service/alarm/alarm-service.h"
#include "../../interfaces/gesture-interface/gesture-interface.h"

// Forward declaration of the service structure
typedef volatile struct gesture_service_t gesture_service_t;

// Define the structure for the gesture service
struct gesture_service_t {
    bool _awake;
    bool _hand_present;
    time_t _last_time;
    gesture_device_t *_gesture_device;
    
    // Function for updating the service
    void (*update)(gesture_service_t *service, alarm_service_t *alarm);
    // Function that sleeps the service
    void (*sleep)(gesture_service_t *service);
    // Function that wakes the service
    void (*wake)(gesture_service_t *service);
};

void gesture_service_init(gesture_service_t *service, gesture_device_t *gesture_device);

#endif // GESTURE_SERVICE_H
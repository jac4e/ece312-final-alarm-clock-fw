#include "gesture-service.h"
#include "../../defines.h"

void gesture_fast(alarm_service_t *alarm) {
    alarm->triggerSnooze(alarm);
}

void gesture_slow(alarm_service_t *alarm){
    alarm->stopAlarm(alarm);
}


// Function to update the gesture service (called during interrupts or timer events)
void gesture_service_update(gesture_service_t *service, alarm_service_t *alarm) {
    time_t t = 0;

    service->_hand_present = service->_gesture_device->read(&t);
    time_t delta = service->_last_time - t;

    if (service->_hand_present) {
        // Hand present, record time and exit function
        service->_last_time = t;
        return;
    }

    if (delta >= GESTURE_SENSOR_SLOW_TIME) {
        gesture_slow(alarm);
    } else {
        gesture_fast(alarm);
    }
}

void gesture_service_wake(gesture_service_t *service){
    service->_awake = true;
    service->_gesture_device->enable_int();
}

void gesture_service_sleep(gesture_service_t *service){
    service->_awake = false;
    service->_gesture_device->disable_int();
}


// Function to initialize the gesture service
void gesture_service_init(gesture_service_t *service, gesture_device_t *gesture_device)
{
    // Initialize service parameters
    service->_awake = true;
    service->_hand_present = false;
    service->_last_time = 0;
    service->_gesture_device = gesture_device;
    
    // Initialize function pointers
    service->update = gesture_service_update;
    service->wake = gesture_service_wake;
    service->sleep = gesture_service_sleep;
}

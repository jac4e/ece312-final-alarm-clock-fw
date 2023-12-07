#ifndef GESTURE_INTERFACE_H
#define GESTURE_INTERFACE_H

// Include any necessary headers here
#include <avr/io.h>
#include <stdbool.h>

#include <time.h>

// Forward declaration of the device structure
typedef volatile struct gesture_device_t gesture_device_t;

// This is the device structure that will be used by the application
struct gesture_device_t {
    // Function pointers for device operations
    bool (*read)(time_t *time);
    void (*disable_int)();
    void (*enable_int)();
};

void gesture_interface_init(gesture_device_t *gesture_device);

#endif // GESTURE_INTERFACE_H

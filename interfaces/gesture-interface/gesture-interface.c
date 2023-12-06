#include "gesture-interface.h"
#include "../../defines.h"

// Function to read from the gesture device
bool gesture_read(time_t *read_time) {
    *read_time = time(NULL);
    return PINC & (1 << GESTURE_SENSOR_PIN); 
}

void gesture_disable_interrupt() {
    PCMSK1 &= ~(1 << GESTURE_SENSOR_PIN);
}

void gesture_enable_interrupt() {
    PCMSK1 |= (1 << GESTURE_SENSOR_PIN);
}

// Function to initialize the gesture device
void gesture_interface_init(gesture_device_t *dev) {
    dev->read = gesture_read;
    dev->disable_int = gesture_disable_interrupt;
    dev->enable_int = gesture_disable_interrupt;


    // Initialize GESTURE_SENSOR_PORT as input
    DDRC &= ~(1 << GESTURE_SENSOR_PIN);

    // Initialize GESTURE_SENSOR_PORT as PCINT1 source
    PCMSK1 |= (1 << GESTURE_SENSOR_PIN);

    // Enable PCINT1 interrupt
    PCICR |= (1 << PCIE1);

}

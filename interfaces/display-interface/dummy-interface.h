#ifndef DUMMY_INTERFACE_H
#define DUMMY_INTERFACE_H

// Include any necessary headers here
#include <avr/io.h>

// This is the device structure that will be used by the application
struct dummy_device {
    // Add any device parameters here
    int dummy_param1;
    // ...

    // Function pointers for device operations
    void (*dummy_init)(struct dummy_device *dev);
    void (*dummy_read)(struct dummy_device *dev);
    void (*dummy_write)(struct dummy_device *dev, uint8_t data);
    // Add more function pointers for other operations
};



#endif // DUMMY_INTERFACE_H

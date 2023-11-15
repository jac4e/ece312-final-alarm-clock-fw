#ifndef DUMMY_SERVICE_H
#define DUMMY_SERVICE_H


// Define the structure for the dummy service
struct dummy_service {
    // Add any service parameters here
    int dummy_param1;
    // ...

    // Function for updating the service
    void (*dummy_update)(struct dummy_service *service); // Can be one single update function, or many functions for different operations during different interupts or timers.
};

#endif // DUMMY_SERVICE_H
#ifndef CLOCK_SERVICE_H
#define CLOCK_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef size_t clock_op_handle_t;

typedef enum
{
    SECOND_OP,
    MINUTE_OP,
    HOUR_OP,
    DAY_OP,
    MONTH_OP,
    YEAR_OP
} op_type;



// Define the structure for the clock service
typedef struct clock_service clock_service;

struct clock_service
{
    // Add any service parameters here
    // Public variables

    // Private variables
    struct tm time; // Not guaranteed to be correct, use get_time() and set_time() instead
    bool is_awake;

    // Array of function pointers for 1 second, 1 minute, 1 hour, 1 day, 1 month, 1 year, etc. operations
    void (*second_ops[16])(clock_service *service, void *data);
    void (*minute_ops[16])(clock_service *service, void *data);
    void (*hour_ops[16])(clock_service *service, void *data);
    void (*day_ops[16])(clock_service *service, void *data);
    void (*month_ops[16])(clock_service *service, void *data);
    void (*year_ops[16])(clock_service *service, void *data);

    // Function pointers for service operations
    // Function for updating the service
    void (*update)(clock_service *service); // Can be one single update function, or many functions for different operations during different interupts or timers.
    // Function that sleeps the service
    void (*sleep)(clock_service *service);
    // Function that wakes the service
    void (*wake)(clock_service *service);
    // Function that adds an operation to the service
    clock_op_handle_t (*add_op)(clock_service *service, void (*op)(struct clock_service *service, void *data), void *data, op_type type);
    // Function to set the time
    void (*set_time)(clock_service *service, struct tm *time);
    // Function to get the time
    void (*get_time)(clock_service *service, struct tm *time);

};

#endif // CLOCK_SERVICE_H
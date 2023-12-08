#ifndef STOPWATCH_SERVICE_H
#define STOPWATCH_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "../clock-service.h"

// Define the structure for the stopwatch service
typedef struct stopwatch_service_t stopwatch_service_t;

struct stopwatch_service_t {
    // time units of the stopwatch
    uint8_t _seconds;
    uint8_t _minutes;
    uint16_t  _hours;
    bool _stopwatchEnable; // high is enables stop watch

  void (*startStopwatch)(stopwatch_service_t *service);
  void (*updateStopwatch)(clock_service *mainClock, stopwatch_service_t *service);
  void (*getStopwatchTime)(stopwatch_service_t *service, struct tm *returnTime);
  void (*stopStopwatch)(stopwatch_service_t *service);
  void (*resetStopwatch)(stopwatch_service_t *service);
};

void initializeStopwatchService(stopwatch_service_t *service);

#endif 
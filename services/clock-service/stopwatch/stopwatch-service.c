#include "stopwatch-service.h"
#include "../../../defines.h"
#include "../clock-service.h"
#include "../../../common.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void stopwatch_service_resetStopwatch(stopwatch_service_t *service){
  service->_hours   = 0;
  service->_minutes = 0;
  service->_seconds = 0;
  service->_stopwatchEnable = true;
}

void stopwatch_service_updateStopwatch(clock_service *mainClock, stopwatch_service_t *service){
  if(service->_stopwatchEnable){
    service->_seconds++;

    if(service->_seconds >= 60){
      service->_seconds -= 60;
      service->_minutes++;

      if(service->_minutes >= 60){
        service->_minutes -= 60;
        service->_hours++;

        if(service->_hours >= 255){
          service->_hours = 0;
        }
      }
    }
  }
}

void stopwatch_service_stopStopwatch(stopwatch_service_t *service){
  service->_stopwatchEnable = 0;
}

void stopwatch_service_getStopwatchTime(stopwatch_service_t *service, struct tm *returnTime){
  returnTime->tm_hour = (int) service->_hours;
  returnTime->tm_min  = (int) service->_minutes;
  returnTime->tm_sec  = (int) service->_seconds;
}

void initializeStopwatchService(stopwatch_service_t *service){
  service->_hours   = 0;
  service->_minutes = 0;
  service->_seconds = 0;
  service->_stopwatchEnable = 0;

  service->resetStopwatch   = stopwatch_service_resetStopwatch;
  service->updateStopwatch  = stopwatch_service_updateStopwatch;
  service->stopStopwatch    = stopwatch_service_stopStopwatch;
  service->getStopwatchTime = stopwatch_service_getStopwatchTime;
}
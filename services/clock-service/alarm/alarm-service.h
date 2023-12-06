#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct alarm_service alarm_service;

typedef enum alarmState {
  disabled,
  idle,
  triggered,
  snooze
};

struct alarm_service {

    // create a structure for a single alarm
    struct _alarm {
      uint8_t hour;
      uint8_t minute;
      uint8_t state;            // stores the state of an alarm
      uint16_t snoozeRemaining; // the amount of time remaining in the snooze state (in seconds)
    };

    // The amount of time in minutes before the alarm should be re-triggered when an alarm enters snooze state
    uint8_t snoozePeriod; 

    // Timer service has 8 alarms
    _alarm _alarms[8];

    // Function for setting an alarm
    void *setAlarm(struct tm *setTime, uint8_t alarmSelection);
    // Checks the alarms to see if any should be triggered
    void *checkAlarms(clock_service *mainClock);
    // disables an alarm
    void *disableAlarm(uint8_t alarmSelection);
};

#endif // DUMMY_SERVICE_H
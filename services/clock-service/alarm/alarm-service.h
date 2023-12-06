#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "../clock-service.h"

typedef volatile struct alarm_service_t alarm_service_t;

typedef enum {
  disabled,
  idle,
  triggered,
  snooze
} alarmState_t;


// create a structure for a single alarm
typedef struct alarm_t alarm_t;
struct alarm_t {
  uint8_t hour;
  uint8_t minute;
  alarmState_t state;            // stores the state of an alarm
  uint16_t snoozeRemaining; // the amount of time remaining in the snooze state (in seconds)
};


struct alarm_service_t {

    // The amount of time in minutes before the alarm should be re-triggered when an alarm enters snooze state
    uint8_t snoozePeriod; 

    // Timer service has 8 alarms
    alarm_t _alarms[8];

    // Consider alarmSelection to be a FLAG, each bit representing one alarm?

    // Function for setting an alarm
    void (*setAlarm)(alarm_service_t *alarm_service, struct tm *setTime, uint8_t alarmSelection);
    // Checks the alarms to see if any should be triggered
    void (*checkAlarms)(alarm_service_t *alarm_service, clock_service *mainClock);
    // disables an alarm
    void (*disableAlarm)(alarm_service_t *alarm_service, uint8_t alarmSelection);
};

void alarm_service_init(alarm_service_t *alarm_service);

#endif // DUMMY_SERVICE_H
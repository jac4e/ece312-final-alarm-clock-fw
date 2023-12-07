#ifndef ALARM_SERVICE_H
#define ALARM_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "../clock-service.h"
#include "../../audio-service/audio-service.h"
// #define ALARM_AMOUNT 8 // the amount of alarms the service supports

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

// Define the structure for the alarm service
typedef volatile struct alarm_service_t alarm_service_t;

struct alarm_service_t {
    
    // stores a pointer to the system audio service
    audio_service *_alarmAudioService;

    // The amount of time in minutes before the alarm should be re-triggered when an alarm enters snooze state
    uint8_t _snoozePeriod; 

    // Timer service has 8 alarms
    alarm_t _alarms[8];

    // sets an alarm
    void (*setAlarm)(alarm_service_t *service, struct tm *setTime, uint8_t alarmSelection);
    // stops an alarm after its been triggered
    void (*stopAlarm)(alarm_service_t *service);
    // triggres an alarm
    void (*triggerAlarm)(alarm_service_t *service, uint8_t selectedAlarm);
    // disables an alarm
    void (*disableAlarm)(alarm_service_t *service, uint8_t alarmSelection);
    // updates the state of the alarm service
    void (*updateAlarmState)(clock_service *mainClock, alarm_service_t *service);

    // puts the alarm in snooze state
    void (*triggerSnooze)(alarm_service_t *service);  
};

void initializeAlarmService(alarm_service_t *service, audio_service *audioService);

#endif // DUMMY_SERVICE_H
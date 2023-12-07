#include "alarm-service.h"
#include "../../../defines.h"
#include "../clock-service.h"
#include "../../audio-service/audio-service.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void alarm_service_setAlarm(alarm_service_t *service, struct tm *setTime, uint8_t alarmSelection){
  
  service->_alarms[alarmSelection].hour   =  (uint8_t) setTime->tm_hour;
  service->_alarms[alarmSelection].minute =  (uint8_t) setTime->tm_min;
  service->_alarms[alarmSelection].state = idle;
  service->_alarms[alarmSelection].snoozeRemaining = 0;

}

void alarm_service_stopAlarm(alarm_service_t *service){
  for(uint8_t i; i < ALARM_AMOUNT; i++){
    if(service->_alarms[i].state == triggered){
      service->_alarms[i].state = idle;
      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void alarm_service_triggerAlarm(alarm_service_t *service, uint8_t alarmSelection){
  service->_alarms[alarmSelection].state = triggered;
}

void alarm_service_disableAlarm(alarm_service_t *service, uint8_t alarmSelection){
  service->_alarms[alarmSelection].state = disabled;
  service->_alarmAudioService->pause(service->_alarmAudioService);
}

void alarm_service_updateAlarmState(alarm_service_t *service, clock_service *mainClock){
  for(uint8_t i; i < ALARM_AMOUNT; i++){
    switch (service->_alarms[i].state){
      case idle:
        if(((uint8_t) mainClock->_time.tm_hour) == service->_alarms[i].hour && ((uint8_t) mainClock->_time.tm_min) == service->_alarms[i].minute){
          service->triggerAlarm(service, i);
        }
      break;
      case snooze:
        if(service->_alarms[i].snoozeRemaining > 0){
          service->_alarms[i].snoozeRemaining--;
        } else {
          service->triggerAlarm(service, i);
        }
      break;
      case triggered:
        service->_alarmAudioService->play(service->_alarmAudioService);
      default:

      break;
    }
  }
}

void alarm_service_triggerSnooze(alarm_service_t *service){
  for(uint8_t i; i < ALARM_AMOUNT; i++){
    if(service->_alarms[i].state == triggered){
      service->_alarms[i].state = snooze;
      service->_alarms[i].snoozeRemaining = (uint16_t) (service->_snoozePeriod * 60);
      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void initalizeAlarmService(alarm_service_t *service, audio_service *audioService) {
  
  // disable all alarms on service
  for(uint8_t i = 0; i < 8; i++){
    service->_alarms[i].state = disabled;
    service->_alarms[i].hour = 0;
    service->_alarms[i].minute = 0;
    service->_alarms[i].snoozeRemaining = 0;
  }

  // set default snooze time of 9 min
  service->_snoozePeriod = 9;

  // stores a pointer to the main audio service as the alarms audio service
  service->_alarmAudioService = audioService;

  //function pointer initilization
  service->setAlarm = alarm_service_setAlarm;
  service->stopAlarm = alarm_service_stopAlarm;
  service->triggerAlarm = alarm_service_triggerAlarm;
  service->disableAlarm = alarm_service_disableAlarm;
  service->updateAlarmState = alarm_service_updateAlarmState;
  service->triggerSnooze = alarm_service_triggerSnooze;
}
#include "alarm-service.h"
#include "../../../defines.h"
#include "../clock-service.h"
#include "../../../common.h"
#include "../../audio-service/audio-service.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void alarm_service_setAlarm(alarm_service_t *service, struct tm *setTime, uint8_t alarmSelection){
  
  // sets the selected alarm to the time inputted
  service->_alarms[alarmSelection].hour   =  (uint8_t) setTime->tm_hour;
  service->_alarms[alarmSelection].minute =  (uint8_t) setTime->tm_min;
  service->_alarms[alarmSelection].state  = alarm_idle;
  service->_alarms[alarmSelection].snoozeMinute = 0;

}

void alarm_service_stopAlarm(alarm_service_t *service){

  // stops any alarms that are triggered
  for(uint8_t i; i < 8; i++){
    if(service->_alarms[i].state == alarm_triggered){
      service->_alarms[i].state = alarm_idle;
      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void alarm_service_triggerAlarm(alarm_service_t *service, uint8_t alarmSelection){

  // triggers the selected alarm
  service->_alarms[alarmSelection].state = alarm_triggered;
  service->_alarmAudioService->play(service->_alarmAudioService);
}

void alarm_service_disableAlarm(alarm_service_t *service, uint8_t alarmSelection){

  // disables the selected alarm
  service->_alarms[alarmSelection].state = alarm_disabled;
  service->_alarmAudioService->pause(service->_alarmAudioService);
}

void alarm_service_updateAlarmState(clock_service *mainClock, alarm_service_t *service){


  if (!service->_is_awake) {
    return;
  }

  // gets the current time from the main clock
  struct tm time_s = {0};
  mainClock->get_time(mainClock, &time_s);

  for(uint8_t i; i < 8; i++){

    // checks the current state of the timer
    switch (service->_alarms[i].state){
      case alarm_idle:{
        // triggers the alarm if the correct time has been reached
        if(((uint8_t) time_s.tm_hour) == service->_alarms[i].hour && ((uint8_t) time_s.tm_min) == service->_alarms[i].minute){
          service->triggerAlarm(service, i);
        }
      break;
      }
      case alarm_snooze:{
        
        // updates the amount of time remaining in snooze state or triggers
        // the alarm if there is no more time lift in snooze state
        if(((uint8_t) time_s.tm_min) == service->_alarms[i].snoozeMinute){
          service->triggerAlarm(service, i);
        }
        
      break;}
      case alarm_triggered:{
        // do nothing
      break;}
      default:{

      break;}
    }
  }
}

void alarm_service_triggerSnooze(alarm_service_t *service){

  struct tm time_s = {0};
  service->_mainClockService->get_time(service->_mainClockService, &time_s);

  // puts an alarm into snooze state if it has been triggered
  for(uint8_t i; i < 8; i++){
    if(service->_alarms[i].state == alarm_triggered){
      service->_alarms[i].state = alarm_snooze;
      service->_alarms[i].snoozeMinute = (uint8_t) ((service->_snoozePeriod) + time_s.tm_min);

      //ensures the snoozeMinute has not overflowed
      if(service->_alarms[i].snoozeMinute >= 60){
        service->_alarms[i].snoozeMinute -= 60;
      }

      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void alarm_service_sleep(alarm_service_t *service){
  service->_is_awake = false;
}

void alarm_service_wake(alarm_service_t *service){
  service->_is_awake = true;
}

void initializeAlarmService(alarm_service_t *service, clock_service *mainClock, audio_service *audioService) {
  
  // disable all alarms on service
  for(uint8_t i = 0; i < 8; i++){
    service->_alarms[i].state = alarm_disabled;
    service->_alarms[i].hour = 0;
    service->_alarms[i].minute = 0;
    service->_alarms[i].snoozeMinute = 0;
  }

  // set default snooze time of 9 min
  service->_snoozePeriod = 9;

  // stores a pointer to the main audio service as the alarms audio service
  service->_alarmAudioService = audioService;
  service->_mainClockService  = mainClock;

  // set the service to awake
  service->_is_awake = true;

  //function pointer initialization
  service->setAlarm = alarm_service_setAlarm;
  service->stopAlarm = alarm_service_stopAlarm;
  service->triggerAlarm = alarm_service_triggerAlarm;
  service->disableAlarm = alarm_service_disableAlarm;
  service->updateAlarmState = alarm_service_updateAlarmState;
  service->triggerSnooze = alarm_service_triggerSnooze;
}
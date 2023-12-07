#include "alarm-service.h"
#include "../../../defines.h"
#include "../clock-service.h"
#include "../../audio-service/audio-service.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void alarm_service_setAlarm(alarm_service_t *service, struct tm *setTime, uint8_t alarmSelection){
  
  // sets the selected alarm to the time inputted
  service->_alarms[alarmSelection].hour   =  (uint8_t) setTime->tm_hour;
  service->_alarms[alarmSelection].minute =  (uint8_t) setTime->tm_min;
  service->_alarms[alarmSelection].state  = idle;
  service->_alarms[alarmSelection].snoozeRemaining = 0;

}

void alarm_service_stopAlarm(alarm_service_t *service){

  // stops any alarms that are triggered
  for(uint8_t i; i < 8; i++){
    if(service->_alarms[i].state == triggered){
      service->_alarms[i].state = idle;
      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void alarm_service_triggerAlarm(alarm_service_t *service, uint8_t alarmSelection){

  // triggers the selected alarm
  service->_alarms[alarmSelection].state = triggered;
  service->_alarmAudioService->play(service->_alarmAudioService);
}

void alarm_service_disableAlarm(alarm_service_t *service, uint8_t alarmSelection){

  // disables the selected alarm
  service->_alarms[alarmSelection].state = disabled;
  service->_alarmAudioService->pause(service->_alarmAudioService);
}

void alarm_service_updateAlarmState(alarm_service_t *service, clock_service *mainClock){

  // gets the current time from the main clock
  struct tm time_s = {0};
  mainClock->get_time(mainClock, &time_s);

  for(uint8_t i; i < 8; i++){

    // ensures that the alarm is only updated every second
      if(mainClock->_service_is_1hz){
        if(mainClock->_counter == 1){
      
        // checks the current state of the timer
        switch (service->_alarms[i].state){
          case idle:
            // triggeres the alarm if the correct time has been reached
            if(((uint8_t) time_s.tm_hour) == service->_alarms[i].hour && ((uint8_t) time_s.tm_min) == service->_alarms[i].minute){
              service->triggerAlarm(service, i);
            }
          break;
          case snooze:
            
            // updates the amount of time remaining in snooze state or triggers
            // the alarm if there is no more time lift in snooze state
            if(service->_alarms[i].snoozeRemaining > 0){
              service->_alarms[i].snoozeRemaining--;
            } else {
              service->triggerAlarm(service, i);
            }
          break;
          case triggered:
            
          default:

          break;
        }
      }
    }
  }
}

void alarm_service_triggerSnooze(alarm_service_t *service){

  // puts an alarm into snooze state if it has been triggered
  for(uint8_t i; i < 8; i++){
    if(service->_alarms[i].state == triggered){
      service->_alarms[i].state = snooze;
      service->_alarms[i].snoozeRemaining = (uint16_t) (service->_snoozePeriod * 60);
      service->_alarmAudioService->pause(service->_alarmAudioService);
    }
  }
}

void initializeAlarmService(alarm_service_t *service, audio_service *audioService) {
  
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

  //function pointer initialization
  service->setAlarm = alarm_service_setAlarm;
  service->stopAlarm = alarm_service_stopAlarm;
  service->triggerAlarm = alarm_service_triggerAlarm;
  service->disableAlarm = alarm_service_disableAlarm;
  service->updateAlarmState = alarm_service_updateAlarmState;
  service->triggerSnooze = alarm_service_triggerSnooze;
}
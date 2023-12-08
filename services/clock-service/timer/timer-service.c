#include "timer-service.h"
#include "../../../defines.h"
#include "../clock-service.h"
#include "../../audio-service/audio-service.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void timer_service_setTimer(timer_service_t *service, clock_service *mainClock, struct tm *setTime, uint8_t timerSelection){

  // gets the current time when the timer is being set
  struct tm time_s = {0};
  mainClock->get_time(mainClock, &time_s);

  // adds the amount of time the timer is set for to the current time
  time_s.tm_hour += setTime->tm_hour;
  time_s.tm_min  += setTime->tm_min ;
  time_s.tm_sec  += setTime->tm_sec ;

  // ensures that seconds has not overflowed
  if(time_s.tm_sec >= 60){
    time_s.tm_sec -= 60;
    time_s.tm_min += 1;
  } 

  // ensures that minutes has not overflowed
  if(time_s.tm_min >= 60){
    time_s.tm_sec -= 60;
    time_s.tm_hour += 1;
  }

  // ensures that hours has not overflowed
  if(time_s.tm_hour >= 24){
    time_s.tm_hour -= 24;
  }
  
  // sets the selected timer to the time inputted
  service->_timers[timerSelection].hour   =  (uint8_t) (time_s.tm_hour);
  service->_timers[timerSelection].minute =  (uint8_t) (time_s.tm_min );
  service->_timers[timerSelection].second =  (uint8_t) (time_s.tm_sec );
  service->_timers[timerSelection].state   = timer_idle;
}

void timer_service_stopTimer(timer_service_t *service){

  // stops any timers that are triggered
  for(uint8_t i; i < 8; i++){
    if(service->_timers[i].state == timer_triggered){
      service->_timers[i].state = timer_idle;
      service->_timerAudioService->pause(service->_timerAudioService);
    }
  }
}

void timer_service_triggerTimer(timer_service_t *service, uint8_t timerSelection){

  // triggers the selected timer
  service->_timers[timerSelection].state = timer_triggered;
  service->_timerAudioService->play(service->_timerAudioService);
}

void timer_service_disableTimer(timer_service_t *service, uint8_t timerSelection){

  // disables the selected timer
  service->_timers[timerSelection].state = timer_disabled;
  service->_timerAudioService->pause(service->_timerAudioService);
}

void timer_service_updateTimerState(clock_service *mainClock, timer_service_t *service){

  // gets the current time from the main clock
  struct tm time_s = {0};
  mainClock->get_time(mainClock, &time_s);

  for(uint8_t i; i < 8; i++){
    // checks the current state of the timer
    switch (service->_timers[i].state){
      case timer_idle:
        // triggeres the timer if the correct time has been reached
        if(((uint8_t) time_s.tm_hour) == service->_timers[i].hour && ((uint8_t) time_s.tm_min) == service->_timers[i].minute && ((uint8_t) time_s.tm_sec) == service->_timers[i].second){
          service->triggerTimer(service, i);
        }
      break;
      case timer_triggered:
        
      default:

      break;
    }
  }
}

void initializeTimerService(timer_service_t *service, audio_service_t*audioService) {
  
  // disable all timers on service
  for(uint8_t i = 0; i < 8; i++){
    service->_timers[i].state  = timer_disabled;
    service->_timers[i].hour   = 0;
    service->_timers[i].minute = 0;
    service->_timers[i].second = 0;
  }

  // stores a pointer to the main audio service as the timers audio service
  service->_timerAudioService = audioService;

  //function pointer initialization
  service->setTimer = timer_service_setTimer;
  service->stopTimer = timer_service_stopTimer;
  service->triggerTimer = timer_service_triggerTimer;
  service->disableTimer = timer_service_disableTimer;
  service->updateTimerState = timer_service_updateTimerState;
}
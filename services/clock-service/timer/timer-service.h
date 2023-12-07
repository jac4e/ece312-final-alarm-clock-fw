#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "../clock-service.h"
#include "../../audio-service/audio-service.h"

typedef enum {
  timer_disabled,
  timer_idle,
  timer_triggered,
} timerState_t;

// create a structure for a single timer
typedef struct timer_t timer_t;
struct timer_t {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  timerState_t state; // stores the state of a timer
};

// Define the structure for the timer service
typedef volatile struct timer_service_t timer_service_t;

struct timer_service_t {
    
    // stores a pointer to the system audio service
    audio_service *_timerAudioService;

    // Timer service has 8 timers
    timer_t _timers[8];

    // sets an timer
    void (*setTimer)(timer_service_t *service, clock_service *mainClock, struct tm *setTime, uint8_t timerSelection);
    // stops a timer after its been triggered
    void (*stopTimer)(timer_service_t *service);
    // triggers a timer
    void (*triggerTimer)(timer_service_t *service, uint8_t selectedTimer);
    // disables a timer
    void (*disableTimer)(timer_service_t *service, uint8_t timerSelection);
    // updates the state of the timer service
    void (*updateTimerState)(clock_service *mainClock, timer_service_t *service);
};

void initializeTimerService(timer_service_t *service, audio_service *audioService);

#endif // DUMMY_SERVICE_H
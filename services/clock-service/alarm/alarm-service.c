#include "alarm-service.h"
#include <avr/cpufunc.h>

void alarm_service_setAlarm(alarm_service_t *alarm_service, struct tm *setTime, uint8_t alarmSelection){
    // Not implemented
    _NOP();
}
// Checks the alarms to see if any should be triggered
void alarm_service_checkAlarms(alarm_service_t *alarm_service, clock_service *mainClock){
    // Not implemented
    _NOP();
}
// disables an alarm
void alarm_service_disableAlarm(alarm_service_t *alarm_service, uint8_t alarmSelection){
    // Not implemented
    _NOP();
}

void alarm_service_init(alarm_service_t *alarm_service) {
    alarm_service->setAlarm = alarm_service_setAlarm;
    alarm_service->checkAlarms = alarm_service_checkAlarms;
    alarm_service->disableAlarm = alarm_service_disableAlarm;
}
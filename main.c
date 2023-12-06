/* 
 * File:   main.c
 * Author: jacques
 *
 * Created on November 14, 2023, 10:44 PM
 */

#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "hd44780.h"
#include "services/clock-service/clock-service.h"
#include "services/clock-service/alarm/alarm-service.h"
#include "services/audio-service/audio-service.h"
#include "services/gesture-service/gesture-service.h"
#include "interfaces/audio-interface/audio-interface.h"
#include "interfaces/gesture-interface/gesture-interface.h"

// Setup Device Globals
static FILE lcd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

/*************************/
/* Interface Definitions */
/*************************/

audio_device audio_device_instance;
gesture_device_t gesture_device;

/***********************/
/* Service Definitions */
/***********************/

volatile clock_service clock_service_instance;
audio_service audio_service_instance;
alarm_service_t alarm_service;
gesture_service_t gesture_service;

/*******************/
/* ISR Definitions */
/*******************/
#if RTC_CRYSTAL_PRESENT
ISR(TIMER2_OVF_vect) {
#else
ISR(TIMER2_COMPA_vect) {
#endif    
    // This ISR will be called when Timer2 overflows (Roughly ever second)
    // Call the clock service update function
    clock_service_instance.update(&clock_service_instance);
    alarm_service.checkAlarms(&alarm_service, &clock_service_instance);
}

ISR(TIMER0_COMPA_vect) {
    // This ISR will be called when Timer0 compare matches
    // Call the audio service update function
    // This is only used for basic audio devices
    audio_service_instance.update(&audio_service_instance);
}

ISR(PCINT1_vect) {
    // This ISR will be called when a change occurs on any of the pins PC0-PC7
    gesture_service.update(&gesture_service, &alarm_service);
}

/****************/
/* Main Program */
/****************/

int main(int argc, char** argv) {
    // disable external clock source (use internal clock source)

    // enables LCD backlight on PB5
    DDRB |= (1 << PB5);
    PORTB |= (1 << PB5);
    
//    ASSR &= ~(1 << AS2);

    // Interface Initialization
    audio_interface_init(&audio_device_instance);
    gesture_interface_init(&gesture_device);

    lcd_init();
    // Service Initialization
    clock_service_init(&clock_service_instance);
    audio_service_init(&audio_service_instance, &audio_device_instance);
    alarm_service_init(&alarm_service);
    gesture_service_init(&gesture_service, &gesture_device);

    sei();
    
    // Main loop
    while (1) {
        // Main program loop
        struct tm time_s = {0};
        clock_service_instance.get_time(&clock_service_instance, &time_s);
        // hour:minute:second
        fprintf(&lcd, "\ec%02u:%02u:%02u", time_s.tm_hour, time_s.tm_min, time_s.tm_sec);
        // day/month/year
        fprintf(&lcd, "\en%02u/%02u/%04u", time_s.tm_mday, time_s.tm_mon, time_s.tm_year + 1900);
        _delay_ms(100);
    }

    return (EXIT_SUCCESS);
}


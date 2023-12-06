/* 
 * File:   main.c
 * Author: jacques
 *
 * Created on November 14, 2023, 10:44 PM
 */
#define F_CPU 16000000UL
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
#include "services/ui-service/ui-service.h"

// Setup Device Globals
//extern FILE lcd;

/*************************/
/* Interface Definitions */
/*************************/

/***********************/
/* Service Definitions */
/***********************/

volatile clock_service clock_service_instance;

volatile ui_data ui_data_instance = {0,1,0};

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
}

ISR(PCINT0_vect){//check buttons
    if(!(PINB & (1<<PINB0))){
        ui_data_instance.button=1;
    }
    if(!(PINB & (1<<PINB1))){
        ui_data_instance.button=2;
    }
    if(!(PINB & (1<<PINB2))){
        ui_data_instance.button=3;
    }
    if(!(PINB & (1<<PINB3))){
        ui_data_instance.button=4;
    }
    
}

/****************/
/* Main Program */
/****************/

int main(int argc, char** argv) {
    
    // Service Initialization
    ui_service_init(&ui_data_instance);
    clock_service_init(&clock_service_instance);
    
    // Interface Initialization
    //lcd_init();
    

    sei();
    
    // Main loop
    while (1) {
        // Main program loop
        struct tm time_s = {0};
        clock_service_instance.get_time(&clock_service_instance, &time_s);
        // hour:minute:second
        //fprintf(&lcd, "\ec%02u:%02u:%02u", time_s.tm_hour, time_s.tm_min, time_s.tm_sec);
        // day/month/year
        //fprintf(&lcd, "\en%02u/%02u/%04u", time_s.tm_mday, time_s.tm_mon, time_s.tm_year + 1900);
        //cli();
        ui_update(&time_s);
        ui_data_instance.button=0;
        //sei();
        
        _delay_ms(100);
    }

    return (EXIT_SUCCESS);
}


#include "../../defines.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <math.h>

#include <time.h>


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../lcd.h"
#include "../../hd44780.h"
#include "ui-service.h"
#include "../clock-service/clock-service.h"
//#include "../clock-service/stopwatch/stopwatch.h"

//#include <string.h>
#include "../clock-service/alarm/alarm-service.h"

struct tm my_time={0};
struct clock_service* clock_service_local;

//static FILE lcd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
ui_data* ui_data_local;
alarm_service_t* alarm_data;
//clock_service * clock_service_local;

static FILE lcd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
struct tm* time_s;

//typedef struct stopwatch_s stopwatch_s;
//stopwatch_s stopwatch_local;




void default_screen_0(){//select b/w clock, timer, stopwatch, setting
    if(ui_data_local->previous_selection!=ui_data_local->current_selection){
        fprintf(&lcd, "\x1B\x01");   
        fprintf(&lcd, "\x1B\x80");
        fprintf(&lcd, "1-Clock 2-Stop");
        fprintf(&lcd, "\x1B\xC0");
        fprintf(&lcd, "3-Timer 4-Setting");
    }
    switch(ui_data_local->button){
        case 1:
            ui_data_local->current_selection=1;
            ui_data_local->previous_selection=0;
            break;
        case 2:
            ui_data_local->current_selection=3;
            ui_data_local->previous_selection=0;
            break;
        case 3:
            ui_data_local->current_selection=5;
            ui_data_local->previous_selection=0;
            break;
        case 4:
            ui_data_local->current_selection=7;
            ui_data_local->previous_selection=0;
            break;
    }
    //ui_data_local->button=0;
}

void clock_standard_1(){
    //CLOCK SCREEN
    if(ui_data_local->previous_selection!=ui_data_local->current_selection){
        fprintf(&lcd, "\x1B\x01"); // clear display screen
        fprintf(&lcd, "\x1B\xC0");
        fprintf(&lcd, "Sle Set Edi Mod");
        //_delay_ms(100);
    }
    ui_data_local->previous_selection=ui_data_local->current_selection;
    fprintf(&lcd, "\x1B\x80");
    fprintf(&lcd, "%02u:%02u:%02u", time_s->tm_hour, time_s->tm_min, time_s->tm_sec);
    
    switch(ui_data_local->button){
        case 1: //sleep
            ui_data_local->current_selection=10;
            ui_data_local->previous_selection=1;
            
            //alarm_data->stopAlarm(alarm_data);
            
            alarm_data->triggerSnooze(alarm_data);
            //alarm_data->triggerAlarm(alarm_data,0);
            
            
            break;
        case 2: //setting
            ui_data_local->current_selection=7;
            ui_data_local->previous_selection=1;
            break;
        case 3: //edit
            ui_data_local->current_selection=2;
            ui_data_local->previous_selection=1;
            break;
        case 4: //mode
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=1;
            break;
    }
    //ui_data_local->button=0;
    
//    fprintf(&lcd, "\x1B\x80");
//    fprintf(&lcd, "button: %i",ui_data_local->button);
//    _delay_ms(1000);
    
}
enum time{
    hour,
    min,
    sec
};
enum time cTime=hour;
enum time aTime=hour;
void clock_edit_2(){
//CLOCK SCREEN EDI PRESSED
    if(ui_data_local->previous_selection!=ui_data_local->current_selection){
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Ups Dow Nex Sav");
    }
    
    fprintf(&lcd, "\x1B\x80");
    fprintf(&lcd, "%02u:%02u:%02u", time_s->tm_hour, time_s->tm_min, time_s->tm_sec);
    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //up
             //up
            switch(cTime){
                case hour:
                    time_s->tm_hour++;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
                case min:
                    time_s->tm_min++;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
                case sec:
                    time_s->tm_sec++;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
            }
            
            //clock_service_set_time_custom(clock_service_local, time_s);
            //mktime(time_s);
             break;
        case 2: //down
             //down
            switch(cTime){
                case hour:
                    time_s->tm_hour--;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
                case min:
                    time_s->tm_min--;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
                case sec:
                    time_s->tm_sec--;
                    mktime(time_s);
                    clock_service_local->set_time(clock_service_local, time_s);
                    break;
            }
             break;
        case 3: //next
             //next
            if(cTime>2){
                cTime=0;
            }else{
                cTime++;
            }
             break;
        case 4: //save
            ui_data_local->current_selection=1;
            ui_data_local->previous_selection=2;
            break;
        }
    
}

void stop_standard_3(){
    if(ui_data_local->previous_selection!=ui_data_local->current_selection){
           //TIMER SCREEN 
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Pla Pau Edi Mod");
    }
    //fprintf(&lcd, "\x1B\x0C");
    fprintf(&lcd, "\x1B\x80");
//    if(stopwatch_local.isPlay==1){
//        stopwatch_tick(&stopwatch_local);
//    }
    //fprintf(&lcd, "%02u:%02u:%02u", stopwatch_local.delta.tm_hour, stopwatch_local.delta.tm_min, stopwatch_local.delta.tm_sec);

    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //play
            //play the timer or pause
            //stopwatch_play(&stopwatch_local);
            break;
        case 2: // pause
            //reset the timer back to 0
            //stopwatch_pause(&stopwatch_local);
            break;
            
        case 3: //edit
            ui_data_local->current_selection=4;
            ui_data_local->previous_selection=3;
            break;
        case 4: //mode
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=3;
            break;
    }


}
void stop_edit_4(){
    
    //if(ui_data_local->previous_selection!=ui_data_local->current_selection){
           //TIMER SCREEN EDI PRESSED
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\x80");
            fprintf(&lcd, "%02u:%02u:%02u", time_s->tm_hour, time_s->tm_min, time_s->tm_sec);
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Ups Dow Nex Sav");
    //}
    ui_data_local->previous_selection=ui_data_local->current_selection;
    switch(ui_data_local->button){
        case 1: //up
            //up
            break;
        case 2: //down
            //down
            break;
        case 3: //next
            //next
            break;
        case 4: //save
            ui_data_local->current_selection=3;
            ui_data_local->previous_selection=4;
            break;
    }
    //ui_data_local->button=0;
    
//            fprintf(&lcd, "\x1B\x80");
//    fprintf(&lcd, "button: %i",ui_data_local->button);
//    _delay_ms(1000);
}

void timer_standard_5(){
    //struct tm timer_s; 
        //if(ui_data_local->previous_selection!=ui_data_local->current_selection){
            //STOPWATCH SCREEN
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\x80");
                //fprintf(&lcd, "%02u:%02u:%02u", time_s->tm_hour-timer_s.tm_hour, time_s->tm_min-timer_s.tm_min, time_s->tm_sec-timer_s.tm_sec);
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Pla Res Edi Mod");
    //}

    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //play
            //play or pause the stopwatch
                
            //timer_s=*time_s;
            //struct stopwatch_s = {time_s, NULL};
            //stopwatch_init(&stopwatch_s);
            break;
        case 2: //reset
            //reset the stopwatch
            break;
        case 3: //edit
            ui_data_local->current_selection=6;
            ui_data_local->previous_selection=5;
            break;
        case 4: //mode
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=5;
            break;
    }
    //ui_data_local->button=0;

}
void timer_edit_6(){
//if(ui_data_local->previous_selection!=ui_data_local->current_selection){
            //STOPWATCH SCREEN EDI PRESSED
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\x80");
            fprintf(&lcd, "%02u:%02u:%02u", time_s->tm_hour, time_s->tm_min, time_s->tm_sec);
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Ups Dow Nex Sav");
    //}
    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //up
            //up
            break;
        case 2: //down
            //down
            break;
        case 3: //next
            //next
            break;
        case 4: //save
            ui_data_local->current_selection=5;
            ui_data_local->previous_selection=6;
            break;
    }
    //ui_data_local->button=0;
}

void settings_main_7(){ //alarm types
    if(ui_data_local->previous_selection!=ui_data_local->current_selection){
    //SETTING SCREEN 
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Lef Rig Sub Mod");
            fprintf(&lcd, "\x1B\x80");
            fprintf(&lcd, "Audio Type: %i", 1);
    
    }
    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //left
            //left
            break;
        case 2: //right
            //right
            break;
        case 3: //submenu
            ui_data_local->current_selection=8;
            ui_data_local->previous_selection=7;
            break;
        case 4: //mode
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=7;
            break;
    }
    //ui_data_local->button=0;
    //}
}


void settings_sub_8(){ //audio types
//if(ui_data_local->previous_selection!=ui_data_local->current_selection){
    //SETTING SCREEN SUB PRESSED
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Lef Rig Edi Bac");
            fprintf(&lcd, "\x1B\x80");
            fprintf(&lcd, "Audio Type: %i", 1);
   // }
    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
        case 1: //left
            //lefft
            break;
        case 2: //right
            //right
            break;
        case 3: //edit
            ui_data_local->current_selection=9;
            ui_data_local->previous_selection=8;
            break;
        case 4: //back
            ui_data_local->current_selection=7;
            ui_data_local->previous_selection=8;
            break;
    }
}
void settings_edit_9(){ //edut type
//if(ui_data_local->previous_selection!=ui_data_local->current_selection){
    //SETTING SCREEN EDI PRESSED
            fprintf(&lcd, "\x1B\x01"); // clear display screen
            fprintf(&lcd, "\x1B\xC0");
            fprintf(&lcd, "Ups Dow Nex Sav");
            fprintf(&lcd, "\x1B\x80");
            //my_time.tm_hour=12;
            //my_time.tm_min=41;
            //my_time.tm_sec=0;
            fprintf(&lcd, "%02u:%02u:%02u", my_time.tm_hour, my_time.tm_min, my_time.tm_sec);

//    }
    ui_data_local->previous_selection=ui_data_local->current_selection;
    
    switch(ui_data_local->button){
            switch(aTime){
                case hour:
                    my_time.tm_hour++;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, my_time);
                    break;
                case min:
                    my_time.tm_min++;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, my_time);
                    break;
                case sec:
                    my_time.tm_sec++;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, time_s);
                    break;
            }
            
            //clock_service_set_time_custom(clock_service_local, time_s);
            //mktime(time_s);
             break;
        case 2: //down
             //down
            switch(aTime){
                case hour:
                    my_time.tm_hour--;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, my_time);
                    break;
                case min:
                    my_time.tm_min--;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, my_time);
                    break;
                case sec:
                    my_time.tm_sec--;
                    mktime(&my_time);
                    //clock_service_local->set_time(clock_service_local, my_time);
                    break;
            }
             break;
        case 3: //next
             //next
            if(aTime>2){
                aTime=0;
            }else{
                aTime++;
            }
             break;
        case 4: //save
            ui_data_local->current_selection=7;
            ui_data_local->previous_selection=9;
            
            //my_time = {0};
            my_time.tm_hour=12;
            my_time.tm_min=41;
            my_time.tm_sec=0;
            alarm_data->setAlarm(alarm_data, &my_time, 0);
            alarm_data->_snoozePeriod = 1;
            break;
}

}


void sleep_10() {
   //if(ui_data_local->previous_selection!=ui_data_local->current_selection){
    fprintf(&lcd, "\x1B\x01");    
    fprintf(&lcd, "\x1B\x85");
        fprintf(&lcd, "SNOOZE");

        ui_data_local->previous_selection=ui_data_local->current_selection; 
        
    //}
    switch(ui_data_local->button){ //all buttons will bring back to menu screen
        case 1: 
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=10;
            break;
        case 2:
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=10;
            break;
        case 3:
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=10;
            break;
        case 4:
            ui_data_local->current_selection=0;
            ui_data_local->previous_selection=10;
            break;
    }
    ui_data_local->button=0;   
}


void ui_service_init(ui_data* ui_data, alarm_service_t* alarm_pass, clock_service* clock_service_instance){
//    stopwatch_local.isPlay=0;
//    stopwatch_local.delta.tm_sec=0;
//    stopwatch_local.delta.tm_min=0;
//    stopwatch_local.delta.tm_hour=0;
    
    ui_data_local=ui_data;
    alarm_data = alarm_pass;
    clock_service_local=clock_service_instance;
    ///clock_service_local=clock_service;
    //Init UI button interrupt
    
    //DDRB &= ~(1 << PB0)&~(1 << PB1)&~(1 << PB2)&~(1 << PB3);
    //PORTB |= (1 << PORTB0)|(1 << PORTB1)|(1 << PORTB2)|(1 << PORTB3);
    
    //DDRB |= (1 << PB0);
    //PORTB |= (1 << PORTB0);
    
    
    
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);
    DDRB &= ~(1 << PB1);
    PORTB |= (1 << PB1);
    DDRB &= ~(1 << PB2);
    PORTB |= (1 << PB2);
    DDRB &= ~(1 << PB3);
    PORTB |= (1 << PB3);
    
    //EICRA |= (1 << ISC01);
    //EICRA |= (1 << ISC01);
    //EICRA &= ~(1 << ISC00);//falling edge
    
    
            
    PCMSK0 |=(1<<PCINT0);
    PCMSK0 |=(1<<PCINT1);
    PCMSK0 |=(1<<PCINT2);
    PCMSK0 |=(1<<PCINT3);
    
    PCICR |=(1<<PCIE0);
    
    //EIMSK |= (1 << INT0);
    //SREG |= (1<<8);
    
    //EIMSK |= (1 << INT0);
    
    WDTCSR=0;
    
//    
//    EICRA |= (1<<ISC01);
//    EICRA &= ~(1<<ISC00);
//    EIMSK |= (1 << INT0);
    
    lcd_init();
    
    default_screen_0();
    //timer_s=&timer_s;
    //*timer_s=malloc(sizeof(struct tm));
    //memcpy(&timer_s, *time_s, sizeof(struct tm));
}
int count=0;
void ui_update(struct tm* time_s_pass){
    time_s = time_s_pass;
    //stopwatch_local.current_time=time_s;
    
    //cli();
    //clock_service_local.get_time(&clock_service_local, &time_s);
    
    //if(ui_data_local->button!=0){
        switch(ui_data_local->current_selection){
            case 0:
                default_screen_0();
                //ui_data_local->previous_selection=0;
                break;
            case 1:
                clock_standard_1();
                break;
            case 2:
                clock_edit_2();
                break;
            case 3:
                stop_standard_3();
                break;
            case 4:
                stop_edit_4();
                break;
            case 5:
                timer_standard_5();
                break;
            case 6:
                timer_edit_6();
                break;
            case 7:
                settings_main_7();
                break;
            case 8:
                settings_sub_8();
                break;
            case 9:
                settings_edit_9();
                break;
            case 10:
                sleep_10();
                break;
        }
        
    //}
//    fprintf(&lcd, "\x1B\xC0");
//    fprintf(&lcd, "button: %i",ui_data_local->button);
        count++;
        if(count<9){
            count=0;
        }
//    fprintf(&lcd, "\x1B\x8F");
//    fprintf(&lcd, "%i",ui_data_local->current_selection);
//    fprintf(&lcd, "\x1B\xCF");
//    fprintf(&lcd, "%i",ui_data_local->button);
//    ui_data_local->button=0;
    //_delay_ms(1000);
    
    
}
#include "clock-service.h"
#include "../../defines.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/usa_dst.h>
#include <math.h>

/**
 * @brief Function to check if the year is a leap year
 *
 * Source: https://github.com/amadeuspzs/atmega328p-rtc/tree/main
 *
 * @param t
 * @return char
 */
static char not_leap(struct tm *t)
{
    if (!(t->tm_year % 100))
    {
        return (char)(t->tm_year % 400);
    }
    else
    {
        return (char)(t->tm_year % 4);
    }
}

/**
 * @brief Function to update the clock service (called during timer 2 overflow ISR)
 *
 * Time keeping code adapted from https://github.com/amadeuspzs/atmega328p-rtc/tree/main to use the tm struct
 *
 * @param service
 */
void clock_service_update(clock_service *service)
{
    if (!service->is_awake)
    {
        return;
    }

#if !(RTC_CRYSTAL_PRESENT)
    if (!service->_is_1hz)
    {

        // Based on the frequency of the clock, we can run our own counter to determine when to update the time at 1Hz
        if (service->_counter++ < service->_frequency)
        {
            return;
        }
        else
        {   
            // resets the service counter to 1
            service->_counter = 1;
        }
    }
#endif

// We can use the libc time functions if we want, or we can implement our own time keeping code
#if USE_LIBC_TIME
    system_tick();
#else
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Time keeping code
        // Put in atomic block to ensure time is updated fully before it is read
        ++service->_time.tm_sec;
        if (service->_time.tm_sec == 60)
        {
            service->_time.tm_sec = 0;
            ++service->_time.tm_min;
            if (service->_time.tm_min == 60)
            {
                service->_time.tm_min = 0;
                ++service->_time.tm_hour;
                if (service->_time.tm_hour == 24)
                {
                    service->_time.tm_hour = 0;
                    // Weekday incrementer
                    ++service->_time.tm_wday;
                    if (service->_time.tm_wday == 7)
                    {
                        service->_time.tm_wday = 0;
                    }

                    // Yearday incrementer
                    ++service->_time.tm_yday;
                    if ((service->_time.tm_yday == 366) && (not_leap(&service->_time)))
                    {
                        service->_time.tm_yday = 0;
                    }
                    else if ((service->_time.tm_yday == 367) && (!not_leap(&service->_time)))
                    {
                        service->_time.tm_yday = 0;
                    }

                    // Monthday, month, and year incrementers
                    ++service->_time.tm_mday;
                    if (service->_time.tm_mday == 32)
                    {
                        service->_time.tm_mon++;
                        service->_time.tm_mday = 1;
                    }
                    else if (service->_time.tm_mday == 31)
                    {
                        if ((service->_time.tm_mon == 4) || (service->_time.tm_mon == 6) || (service->_time.tm_mon == 9) || (service->_time.tm_mon == 11))
                        {
                            service->_time.tm_mon++;
                            service->_time.tm_mday = 1;
                        }
                    }
                    else if (service->_time.tm_mday == 30)
                    {
                        if (service->_time.tm_mon == 2)
                        {
                            service->_time.tm_mon++;
                            service->_time.tm_mday = 1;
                        }
                    }
                    else if (service->_time.tm_mday == 29)
                    {
                        if ((service->_time.tm_mon == 2) && (not_leap(&service->_time)))
                        {
                            service->_time.tm_mon++;
                            service->_time.tm_mday = 1;
                        }
                    }
                    if (service->_time.tm_mon == 13)
                    {
                        service->_time.tm_mon = 1;
                        service->_time.tm_year++;
                    }
                }
            }
        }
        // end of time keeping code
    }
#endif
}

void clock_service_sleep(clock_service *service)
{
    service->is_awake = false;
}

void clock_service_wake(clock_service *service)
{
    service->is_awake = true;
}

/**
 * @brief Function to add an operation to the service at a specified interval (1 second, 1 minute, 1 hour, 1 day, 1 month, 1 year)
 *
 * @param service  The clock service
 * @param op     A pointer to the operation function
 * @param data  The data to pass to the operation
 * @param type  The interval at which to call the operation (1 second, 1 minute, 1 hour, 1 day, 1 month, 1 year). See op_type enum.
 * @return size_t
 */
clock_op_handle_t clock_service_add_op(clock_service *service, void (*op)(clock_service *service, void *data), void *data, op_type type)
{
    size_t i = 0;
    switch (type)
    {
    case SECOND_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->second_ops[i] == NULL)
            {
                service->second_ops[i] = op;
                break;
            }
        }
        break;
    case MINUTE_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->minute_ops[i] == NULL)
            {
                service->minute_ops[i] = op;
                break;
            }
        }
        break;
    case HOUR_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->hour_ops[i] == NULL)
            {
                service->hour_ops[i] = op;
                break;
            }
        }
        break;
    case DAY_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->day_ops[i] == NULL)
            {
                service->day_ops[i] = op;
                break;
            }
        }
        break;
    case MONTH_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->month_ops[i] == NULL)
            {
                service->month_ops[i] = op;
                break;
            }
        }
        break;
    case YEAR_OP:
        for (i = 0; i < 16; i++)
        {
            if (service->year_ops[i] == NULL)
            {
                service->year_ops[i] = op;
                break;
            }
        }
        break;
    default:
        break;
    }
}

void clock_service_set_time_custom(clock_service *service, struct tm *time_s)
{
    service->_time = *time_s;
}

void clock_service_get_time_custom(clock_service *service, struct tm *time_s)
{
    *time_s = service->_time;
}

void clock_service_set_time_libc(clock_service *service, struct tm *time_s)
{
    set_system_time(mktime((struct tm *)time_s));
}

void clock_service_get_time_libc(clock_service *service, struct tm *time_s)
{
    time_t t = time(NULL);
    localtime_r(&t, time_s);
}

const uint16_t prescaler_values[] = {0, 1, 8, 32, 64, 128, 256, 1024};

// Function to initialize the clock service
void clock_service_init(clock_service *service)
{
    // Get build time from __DATE__ and __TIME__ macros
    // These macros are defined by the compiler at compile time
    // Format: "Mmm dd yyyy" and "hh:mm:ss"
    // Example: "Feb 13 2021" and "12:00:00"

    char build_date[12] = __DATE__;
    char build_time[9] = __TIME__;


    char month[4] = {0};
    char day[3] = {0};
    char year[5] = {0};
    char hour[3] = {0};
    char minute[3] = {0};
    char second[3] = {0};

    // Get month
    month[0] = build_date[0];
    month[1] = build_date[1];
    month[2] = build_date[2];

    // Get day
    day[0] = build_date[4];
    day[1] = build_date[5];

    // Get year
    year[0] = build_date[7];
    year[1] = build_date[8];
    year[2] = build_date[9];
    year[3] = build_date[10];

    // Get hour
    hour[0] = build_time[0];
    hour[1] = build_time[1];

    // Get minute
    minute[0] = build_time[3];
    minute[1] = build_time[4];

    // Get second
    second[0] = build_time[6];
    second[1] = build_time[7];

    // Convert strings to integers and store in service->_time
    service->_time.tm_sec = atoi(second);
    service->_time.tm_min = atoi(minute);
    service->_time.tm_hour = atoi(hour);
    service->_time.tm_mday = atoi(day);
    service->_time.tm_wday = 0;
    service->_time.tm_mon = 0;
    service->_time.tm_year = atoi(year) - 1900;
    service->_time.tm_yday = 0;
    service->_time.tm_isdst = 0;

    // Convert month string to integer
    if (strcmp(month, "Jan") == 0)
    {
        service->_time.tm_mon = 0;
    }
    else if (strcmp(month, "Feb") == 0)
    {
        service->_time.tm_mon = 1;
    }
    else if (strcmp(month, "Mar") == 0)
    {
        service->_time.tm_mon = 2;
    }
    else if (strcmp(month, "Apr") == 0)
    {
        service->_time.tm_mon = 3;
    }
    else if (strcmp(month, "May") == 0)
    {
        service->_time.tm_mon = 4;
    }
    else if (strcmp(month, "Jun") == 0)
    {
        service->_time.tm_mon = 5;
    }
    else if (strcmp(month, "Jul") == 0)
    {
        service->_time.tm_mon = 6;
    }
    else if (strcmp(month, "Aug") == 0)
    {
        service->_time.tm_mon = 7;
    }
    else if (strcmp(month, "Sep") == 0)
    {
        service->_time.tm_mon = 8;
    }
    else if (strcmp(month, "Oct") == 0)
    {
        service->_time.tm_mon = 9;
    }
    else if (strcmp(month, "Nov") == 0)
    {
        service->_time.tm_mon = 10;
    }
    else if (strcmp(month, "Dec") == 0)
    {
        service->_time.tm_mon = 11;
    }

// We can use the libc time functions if we want, or we can implement our own time keeping code
#if USE_LIBC_TIME
    // set time zone to default mountain time
    set_zone(-7 * ONE_HOUR);
    // set_zone(0);
    // set daylight savings time to usa_dst function
    set_dst(usa_dst);
    // Be careful with this function, if set to anythin less than -7*ONE_HOUR, the local time will underflow to the year 2136
    set_system_time(mktime(&service->_time));
#endif
    service->is_awake = true;
    service->_clock_prescaler = 0xFFFF;
    service->_clock_top = 0xFFFF;
    service->_frequency = 0xFFFF;
    service->_counter = 1;
    service->_is_1hz = true;

    // Initialize function pointers
    service->update = clock_service_update;
    service->sleep = clock_service_sleep;
    service->wake = clock_service_wake;
    service->add_op = clock_service_add_op;

#if USE_LIBC_TIME
    service->set_time = clock_service_set_time_libc;
    service->get_time = clock_service_get_time_libc;
#else
    service->set_time = clock_service_set_time_custom;
    service->get_time = clock_service_get_time_custom;
#endif

// Implement any additional initialization here

// Initialize Timer2 for 1Hz clock based on the actual clock source
#if RTC_CRYSTAL_PRESENT
    // Setup timer2 for 1HZ using external 32.768 kHz crystal as a clock source

    _delay_ms(1000);                    // wait for external crystal to stabilize
    TCCR2B = 0;                         // stop Timer 2
    TIMSK2 = 0;                         // disable Timer 2 interrupts
    ASSR = (1 << AS2);                  // select asynchronous operation of Timer2
    TCNT2 = 0;                          // clear Timer 2 counter
    TCCR2A = 0;                         // normal count up mode, no port output
    TCCR2B = (1 << CS22) | (1 << CS20); // select prescaler 128 => 1 sec between each overflow

    while (ASSR & ((1 << TCN2UB) | (1 << TCR2BUB)))
        ; // wait for TCN2UB and TCR2BUB to clear

    TIFR2 = 0xFF;          // clear all interrupt flags
    TIMSK2 = (1 << TOIE2); // enable Timer2 overflow interrupt

#else
    // Setup timer2 for 1Hz (or whatever is the slowest possible) using F_CPU as a clock source (time won't be accurate as 1Hz not possible with F_CPU=16MHz)

    // Caclulate the timer prescaler value and the compare match value

    for (service->_clock_prescaler = 0; service->_clock_prescaler < 8; service->_clock_prescaler++)
    {
        service->_clock_top = EXT_CLK / (1 * (uint64_t)prescaler_values[service->_clock_prescaler]) - 1;
        if (service->_clock_top <= 255)
        {
            break;
        }
    }


    // Setup timer2 for 1Hz (or whatever is the slowest possible whole frequency) using EXT_CLK as a clock source 
    // Caclulate the timer prescaler value and the compare match value so that the service frequncy is a whole number
    if (service->_clock_top > 255)
    {
        
        // determines the largest pre-scaler value that is a factor of MCUs clock
        for(uint8_t i = 7; i > 0; i--){
            if((EXT_CLK % prescaler_values[i]) == 0){      
                
                // sets the services pre-scaler value
                service->_clock_prescaler = i;
                
                // sets the service frequency to the pre-scaled clock frequency
                service->_frequency = EXT_CLK / prescaler_values[i];
                break;
            }
        } 
        
        // determines the largest 8 bit value that is a factor of the 
        // pre-scaled EXT_CLK clock 
        for(uint8_t i = 255; i > 0; i--){
            if((service->_frequency % i) == 0){
                
                // sets the clock top value to the largest factor of the 
                // pre-scaled clock - 1
                service->_clock_top = i - 1;
                break;
            }
            
        }
         
        // sets the service frequency
         service->_frequency /= (service->_clock_top + 1);
        service->_is_1hz = false;
    };

    TCCR2B = 0;                                   // stop Timer 2
    TIMSK2 = 0;                                   // disable Timer 2 interrupts
    TCNT2 = 0;                                    // clear Timer 2 counter
    TCCR2A = (1 << WGM21);                        // CTC mode, no port output
    TCCR2B = (0b111 & service->_clock_prescaler); // Set prescaler
    OCR2A = service->_clock_top;                  // set CTC compare value

    TIFR2 = 0xFF;           // clear all interrupt flags
    TIMSK2 = (1 << OCIE2A); // enable Timer2 compare match A interrupt

#endif
}
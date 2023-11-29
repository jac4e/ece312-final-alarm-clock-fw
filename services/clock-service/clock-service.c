#include "clock-service.h"
#include "../../defines.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <util/usa_dst.h>

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

// We can use the libc time functions if we want, or we can implement our own time keeping code
#if USE_LIBC_TIME
    system_tick();
#else
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Time keeping code
        // Put in atomic block to ensure time is updated fully before it is read
        ++service->time.tm_sec;
        if (service->time.tm_sec == 60)
        {
            service->time.tm_sec = 0;
            ++service->time.tm_min;
            if (service->time.tm_min == 60)
            {
                service->time.tm_min = 0;
                ++service->time.tm_hour;
                if (service->time.tm_hour == 24)
                {
                    service->time.tm_hour = 0;
                    // Weekday incrementer
                    ++service->time.tm_wday;
                    if (service->time.tm_wday == 7)
                    {
                        service->time.tm_wday = 0;
                    }

                    // Yearday incrementer
                    ++service->time.tm_yday;
                    if ((service->time.tm_yday == 366) && (not_leap(&service->time)))
                    {
                        service->time.tm_yday = 0;
                    }
                    else if ((service->time.tm_yday == 367) && (!not_leap(&service->time)))
                    {
                        service->time.tm_yday = 0;
                    }

                    // Monthday, month, and year incrementers
                    ++service->time.tm_mday;
                    if (service->time.tm_mday == 32)
                    {
                        service->time.tm_mon++;
                        service->time.tm_mday = 1;
                    }
                    else if (service->time.tm_mday == 31)
                    {
                        if ((service->time.tm_mon == 4) || (service->time.tm_mon == 6) || (service->time.tm_mon == 9) || (service->time.tm_mon == 11))
                        {
                            service->time.tm_mon++;
                            service->time.tm_mday = 1;
                        }
                    }
                    else if (service->time.tm_mday == 30)
                    {
                        if (service->time.tm_mon == 2)
                        {
                            service->time.tm_mon++;
                            service->time.tm_mday = 1;
                        }
                    }
                    else if (service->time.tm_mday == 29)
                    {
                        if ((service->time.tm_mon == 2) && (not_leap(&service->time)))
                        {
                            service->time.tm_mon++;
                            service->time.tm_mday = 1;
                        }
                    }
                    if (service->time.tm_mon == 13)
                    {
                        service->time.tm_mon = 1;
                        service->time.tm_year++;
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
    service->time = *time_s;
}

void clock_service_get_time_custom(clock_service *service, struct tm *time_s)
{
    *time_s = service->time;
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
    // Initialize service parameters

    // Initialize time to epoch Midnight Jan 1 2000
    service->time.tm_sec = 0;
    service->time.tm_min = 0;
    service->time.tm_hour = 0;
    service->time.tm_mday = 0;
    service->time.tm_wday = 0;
    service->time.tm_mon = 0;
    service->time.tm_year = 100;
    service->time.tm_yday = 0;
    service->time.tm_isdst = 0;

// We can use the libc time functions if we want, or we can implement our own time keeping code
#if USE_LIBC_TIME
    // set time zone to default mountain time
    set_zone(-7 * ONE_HOUR);
    // set_zone(0);
    // set daylight savings time to usa_dst function
    set_dst(usa_dst);
    // set system time default based on localtime 7*ONE_HOUR
    // if set to 0 (GMT epoch) it will be 6 hours off underflowing to Feb 6 2136 when retrieving the time
    set_system_time(7 * ONE_HOUR);
#endif
    service->is_awake = true;
    // ...

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
    uint8_t clock_prescaler = 0;
    uint64_t clock_top = 0;

    for (clock_prescaler = 0; clock_prescaler < 8; clock_prescaler++)
    {
        clock_top = F_CPU / (2 * 1 * (uint64_t)prescaler_values[clock_prescaler]) - 1;
        if (clock_top <= 255)
        {
            break;
        }
    }

    // If the clock top is greater than 255, we could not find a prescaler value that would work for 1Hz
    // So we set the clock top to 255 and set the prescaler to 1024 to get the slowest possible clock
    if (clock_top > 255)
    {
        clock_top = 255;
        clock_prescaler = 7;
    }

    TCCR2B = 0;                         // stop Timer 2
    TIMSK2 = 0;                         // disable Timer 2 interrupts
    TCNT2 = 0;                          // clear Timer 2 counter
    TCCR2A = (1 << WGM21);              // CTC mode, no port output
    TCCR2B = (0b111 & clock_prescaler); // Set prescaler
    OCR2A = clock_top;                  // set CTC compare value

    TIFR2 = 0xFF;           // clear all interrupt flags
    TIMSK2 = (1 << OCIE2A); // enable Timer2 compare match A interrupt

#endif
}
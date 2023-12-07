/**
 * @file defines.h
 * @brief This is the board specific header file. It specifies the features of the board so that the code can be compiled for different boards.
 */

#ifndef DEFINES_H
#define DEFINES_H

/**********************/
/* Test Mode Settings */
/**********************/

#define TEST_MODE 1 // Set to 1 to enable test mode, 0 to disable test mode

// Test sections definitions
#define TEST_ALARM 0
#define TEST_AUDIO_BASIC 1
#define TEST_AUDIO_PREMIUM 2
#define TEST_GESTURE 3
#define TEST_UI 4

#if TEST_MODE
#define TEST_SECTION TEST_ALARM
#endif // TEST_MODE

/*******************/
/* Pin Definitions */
/*******************/

#define AUDIO_ID_PORT PC0
#define AUDIO_PWM_AMP_SHDN_PORT PD6

#define GESTURE_SENSOR_PIN PC3

/* HD44780 LCD port connections */
#define HD44780_RS D, 7
#define HD44780_RW D, 5
#define HD44780_E  D, 4
/* The data bits have to be not only in ascending order but also consecutive. */
#define HD44780_D4 D, 0

/**********************/
/* System Definitions */
/**********************/

/* UART baud rate */
#define UART_BAUD  9600

#define AUDIO_DEVICE_ADDR 0x00 // TODO: Add the correct address here
#define F_CPU 16000000UL // For Xplained Mini (16 MHz external clock)
// #define F_CPU 8000000UL // For ATmega328P (8 MHz internal clock)
// #define F_CPU 1000000UL // For ATmega328P (1 MHz internal clock)

#define RTC_CRYSTAL_PRESENT 0 // Set to 1 if RTC crystal is present, 0 otherwise
#define ONEMHZ_CRYSTAL_PRESENT 0 // Set to 1 if 1 MHz crystal is present, 0 otherwise
#define USE_LIBC_TIME 1 // Set to 1 to use the time.h libraries built in time keeping functions, 0 to use custom time keeping service update function

#define CLOCK_COUNTER_PRECISION 1000 // The precision of the clock counter when 1Hz cannot be achieved without software correction

#if RTC_CRYSTAL_PRESENT
#define EXT_CLK 32768
#elif ONEMHZ_CRYSTAL_PRESENT
#define EXT_CLK 1000000
#else
#define EXT_CLK F_CPU
#endif

#define GESTURE_SENSOR_SLOW_TIME 5 // Slow hand gesture is 10 or more seconds

#endif // DEFINES_H
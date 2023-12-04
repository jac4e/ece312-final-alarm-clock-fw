/**
 * @file defines.h
 * @brief This is the board specific header file. It specifies the features of the board so that the code can be compiled for different boards.
 */

#ifndef DEFINES_H
#define DEFINES_H

#define F_CPU 16000000UL // For Xplained Mini (16 MHz external clock)
// #define F_CPU 8000000UL // For ATmega328P (8 MHz internal clock)
// #define F_CPU 1000000UL // For ATmega328P (1 MHz internal clock)

/* UART baud rate */
#define UART_BAUD  9600

/* HD44780 LCD port connections */
#define HD44780_RS D, 7
#define HD44780_RW D, 5
#define HD44780_E  D, 4
/* The data bits have to be not only in ascending order but also consecutive. */
#define HD44780_D4 D, 0

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

#endif // DEFINES_H
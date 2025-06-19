#ifndef GPSDO__FIRMWARE__CONFIG__H
#define GPSDO__FIRMWARE__CONFIG__H

#include <pico/stdlib.h>

#include <hardware/uart.h>
#include <hardware/i2c.h>
#include <hardware/pio.h>

#define LOG_UART                                    uart1
#define LOG_UART_SPEED                              115200
#define LOG_UART_PIN_TX                             4
#define LOG_UART_PIN_RX                             5
#define LOG_UART_DATA_BITS                          8
#define LOG_UART_STOP_BITS                          1
#define LOG_UART_PARITY                             UART_PARITY_NONE

#define GPS_UART                                    uart0
#define GPS_UART_SPEED                              9600
#define GPS_UART_PIN_TX                             16
#define GPS_UART_PIN_RX                             17
#define GPS_UART_DATA_BITS                          8
#define GPS_UART_STOP_BITS                          1
#define GPS_UART_PARITY                             UART_PARITY_NONE
#define GPS_UART_IRQ                                UART0_IRQ

// #define COUNTER_PIO                                 pio0
// #define COUNTER_STATE_MACHINE                       0
#define COUNTER_PIN_CLOCK                           3
#define COUNTER_PIN_PPS                             22

#define OSCILLATOR_I2C                              i2c0
#define OSCILLATOR_I2C_PIN_SDA                      12
#define OSCILLATOR_I2C_PIN_SCL                      13
#define OSCILLATOR_I2C_BAUDRATE                     400000
#define OSCILLATOR_I2C_ADDRESS                      0x60
#define OSCILLATOR_I2C_XTAL_FREQUENCY               25000000
#define OSCILLATOR_I2C_XTAL_FREQUENCY_CORRECTION    0
#define OSCILLATOR_FREQUENCY_CLK0                   40000000
#define OSCILLATOR_FREQUENCY_CLK1                   25000000
#define OSCILLATOR_FREQUENCY_CLK2                   10000000

#define LED_PIN_GPS_DATA                            6
#define LED_PIN_GPS_FIX                             7
#define LED_PIN_GPS_PPS                             8
#define LED_PIN_CLOCK_SYNC                          9

#define LCD_PIN_RS                                  14
#define LCD_PIN_ENABLE                              15
#define LCD_PIN_DATA_4                              21
#define LCD_PIN_DATA_5                              20
#define LCD_PIN_DATA_6                              19
#define LCD_PIN_DATA_7                              18

#define REFERENCE_CLOCK_FREQUENCY                   OSCILLATOR_FREQUENCY_CLK2
#define REFERENCE_CLOCK_DRIFT_PARAMETER             4

#endif

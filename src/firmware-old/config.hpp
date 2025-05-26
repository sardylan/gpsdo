#ifndef CONFIG_H
#define CONFIG_H

#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/i2c.h>

#define GPS_UART                uart0
#define GPS_UART_SPEED          9600
#define GPS_UART_PIN_TX         16
#define GPS_UART_PIN_RX         17
#define GPS_UART_DATA_BITS      8
#define GPS_UART_STOP_BITS      1
#define GPS_UART_PARITY         UART_PARITY_NONE
#define GPS_UART_IRQ            UART0_IRQ

#define OSCILLATOR_I2C          i2c1
#define OSCILLATOR_I2C_PIN_SDA  19
#define OSCILLATOR_I2C_PIN_SCL  20
#define OSCILLATOR_I2C_BAUDRATE 400000

#define LED_PIN_GPS_DATA        6
#define LED_PIN_GPS_FIX         7
#define LED_PIN_GPS_PPS         8
#define LED_PIN_CLOCK_SYNC      9

#endif

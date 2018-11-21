#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include "board.h"
#include "constants.h"

typedef enum uart_rx_status
{
    WAITING,
    STARTED,
    DONE,
    BUFFER_FULL
}uart_rx_status_t;

typedef enum periphery
{
    PC,
    GSM,
    LCD
}periphery_t;

typedef struct date_t
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

    int8_t zone;
}date_t;

typedef struct uart_rx
{
    unsigned char     buffer[UART_BUFFER_SIZE];
    unsigned char     begin;
    unsigned char     end;
    uint16_t          index;
    uint8_t           occurrence;
    uint8_t           occured;
    uart_rx_status_t  status;
}uart_rx_t;

typedef struct device_status
{
    uint8_t  LCD_pwr;         // LCD power status
    uint8_t  GSM_pwr;         // GSM power status
    uint8_t  GSM_configured;
    uint8_t  LCD_configured;
    uint8_t  is_date_displayed;
}device_status_t;

typedef struct timer_variables
{
    uint8_t  ovf_cnt;
    uint8_t  cnt_max;
    uint8_t  time_out;
    //uint16_t sec_cnt;
}timer_variables_t;

typedef struct interrupt_flags
{
    uint8_t alarm_button;
    uint8_t alarm_sw;
    uint8_t GSM;
    uint8_t timeout;
}interrupt_flags_t;

typedef struct SMS
{
    uint8_t   slot;
    char      sender[12];
    char      date_str[9];
    char      time_str[9];
    char      zone_str[4];
    char      text[161];
    date_t    time;
}SMS_t;

#endif

#ifndef BOARD_H
#define BOARD_H

//#define  F_CPU 1000000UL
#define  F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "GSM.h"
#include "LCD.h"
#include "time.h"
#include "data_types.h"
#include "constants.h"

#define BAUD 57600                              // define baud
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)        // set baud rate value for UBRR

#define CLK_8MHZ         0xbb                    // calibration value for 8MHz clock
#define CLK_1MHZ         0xc5                    // calibration value for 1MHz clock

/**** define output pins ****/
#define LED2             PF2
#define LED1             PF3

#define BUZZER           PB6

#define HEADER0          PC0
#define HEADER1          PC1
#define HEADER2          PC2
#define HEADER3          PC3
#define HEADER4          PC4
#define HEADER5          PC5
#define HEADER6          PC6
#define HEADER7          PC7

/**** define input pins ****/
#define SW2              PE2
#define SW1              PE3
#define SW_ALRM          PE4
#define BUTT_ALRM        PE6

#define BUTT2            PG3
#define BUTT1            PG4

/**** define GSM output pins ****/
#define GSM_RI           PE5

#define GSM_RTS          PD4
#define GSM_ON_OFF       PD6
#define GSM_EM_OFF       PD7

/**** define GSM input pins *****/
#define GSM_CTS          PD5

/**** define LCD output pins ****/
#define LCD_D0           PA0
#define LCD_D1           PA1
#define LCD_D2           PA2
#define LCD_D3           PA3
#define LCD_D4           PA4
#define LCD_D5           PA5
#define LCD_D6           PA6
#define LCD_D7           PA7

#define LCD_DATA         PORTA   // LCD data port
#define RS               PF0       //PF0
#define En               PF1       //PF1

#define LCD_ON_OFF       PB7

/**** define RF output pins *****/
#define SPI_SS           PB0
#define SPI_CLK          PB1
#define SPI_MOSI         PB2
#define SPI_MISO         PB3
#define RF_CS            PB4
#define RF_CE            PB5

/**** define RF input pins  *****/
#define RF_IRQ           PE7

/******* ISR rutin names *********/
#define ALARM_BUTT_ISR   INT6_vect
#define ALARM_SW_ISR     INT4_vect
#define GSM_RI_ISR       INT5_vect

/*   global variables */
extern volatile interrupt_flags_t int_flags;
extern volatile timer_variables_t timer1;
extern volatile device_status_t   dev_stat;     // general device information
extern volatile uart_rx_t         GSM_uart;     // UART buffer variables
extern SMS_t                      message;
extern date_t                     date;
extern uint8_t                    new_msg_received;

/*   periphery init functions   */
void          init_board(void); // buttons, switches, buzzer, LEDs,

uint8_t       button(char button);

void          switch_LED(char id, uint8_t state);
void          switch_buzzer(uint8_t state);

void          PC_send_str(char* cmd);
void          PC_send_int(uint8_t num);

void          print(periphery_t target, const char *fmt, ...);

void          set_time_out(uint8_t value);
void          init_global_variables();
void          interrupt_handler(uint8_t* flag, void (*handler_func)(void));

#endif

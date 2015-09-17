#ifndef GSM_H
#define GSM_H

#include "board.h"
#include "data_types.h"

/******* AT commands ********/

#define cmd_GSM_test			(char*){"AT"}
#define cmd_echo_mode			(char*){"ATE0"}
#define cmd_SMS_format			(char*){"AT+CMGF"}     //1
#define cmd_send_SMS			(char*){"AT+CMGS"}	   //00number
#define cmd_read_SMS			(char*){"AT+CMGR"}
#define cmd_sync_time			(char*){"AT+CTZU"}     //3
#define cmd_sync_status			(char*){"AT+CTZU?\r"}
#define cmd_get_time			(char*){"AT+CCLK"}
#define cmd_alarm				(char*){"AT+QALARM"}
#define cmd_list_SMS			(char*){"AT+CMGL=\"ALL\",0\r"}
#define cmd_del_all_SMS			(char*){"AT+QMGDA=\"DEL ALL\"\r"}

/** GSM state EEPROM address **/
//#define GSM_st_add		1

/******** prototypes ********/

uint8_t		  GSM_get_state();
uint8_t		  get_SMS_slot(char* buffer);
void          read_slot(uint8_t slot);

uint8_t       parse_SMS(char* data, SMS_t* msg);

void          init_GSM(void);
void 		  init_GSM_uart(void);
uint8_t		  config_GSM(void);

unsigned char GSM_uart_recieve(void);
void          GSM_uart_transmit(unsigned char data);

void		  GSM_send_cmd(char* cmd, char* param);
void          GSM_send_str(char* cmd);
uint8_t		  GSM_get_str(char start, char end, uint8_t value);

void          GSM_process_response();

uint8_t	 	  GSM_send_SMS(char* text,char* number);
uint8_t		  GSM_set_alarm(uint8_t state, char* date, uint8_t repeat, uint8_t power);

uint8_t       wait_for(char c, uint8_t value);
void          switch_GSM(void);
void          GSM_em_shut_down(void);

#endif

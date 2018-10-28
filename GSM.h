#ifndef GSM_H
#define GSM_H

#include "board.h"
#include "data_types.h"

/******* AT commands ********/

#define cmd_GSM_test            (char*){"AT"}
#define cmd_echo_mode           (char*){"ATE0"}
#define cmd_SMS_format          (char*){"AT+CMGF"}     //1
#define cmd_send_SMS            (char*){"AT+CMGS"}     //00number
#define cmd_read_SMS            (char*){"AT+CMGR"}
#define cmd_sync_time           (char*){"AT+CTZU"}     //3
#define cmd_sync_status         (char*){"AT+CTZU?\r"}
#define cmd_get_time            (char*){"AT+CCLK"}
#define cmd_alarm               (char*){"AT+QALARM"}
#define cmd_list_SMS            (char*){"AT+CMGL=\"ALL\",0\r"}
#define cmd_del_all_SMS         (char*){"AT+QMGDA=\"DEL ALL\"\r"}

/******** prototypes ********/
void        init_GSM(void);
void        GSM_send_cmd(char* cmd, char* param);
void        GSM_send_str(char* cmd);
void        GSM_process_response();

uint8_t     GSM_send_SMS(char* text,char* number);
uint8_t     parse_SMS(char* data, SMS_t* msg);
void        get_SMS_slot(char* buffer, uint8_t* slot);
void        read_slot(uint8_t slot);

uint8_t     wait_for(char c, uint8_t value);
uint8_t     GSM_get_state();
void        switch_GSM(void);
void        GSM_em_shut_down(void);

#endif

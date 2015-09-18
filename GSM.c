#include "board.h"

uint8_t GSM_get_state()
{
    print(GSM,"%s\r",cmd_GSM_test);
    if(wait_for('K', 36))
    {
        return ON;
    }
    else
    {
        return OFF;
    }
}

/***************************************************/
void init_GSM()
{
    DDRD |= (1<<GSM_RTS) | (1<<GSM_ON_OFF) | (1<<GSM_EM_OFF);    // outputs
    DDRD &= ~(1<<GSM_CTS);                                        // inputs
    PORTD |= (1<<GSM_ON_OFF) | (1<<GSM_EM_OFF);                    // default high

    init_GSM_uart();
}

/***************************************************/
void init_GSM_uart()
{
    UBRR1H = (BAUDRATE>>8);                      // shift the register right by 8 bits
    UBRR1L = BAUDRATE;                           // set baud rate
    UCSR1B |= (1<<TXEN1)|(1<<RXEN1);              // enable receiver, transmitter
    UCSR1C |= (1<<UCSZ10)|(1<<UCSZ11);            // 8bit data format
    UCSR1B |= (1<<RXCIE1);                         // enable receive complete interrupts (GPS uart)
}

/***************************************************/
uint8_t config_GSM()
{
    print(GSM,"%s\r",cmd_echo_mode);
    wait_for('K', 36);                              // wait for OK or ERROR
    print(GSM,"%s=%d\r",cmd_SMS_format,1);
    wait_for('K', 36);                              // wait for OK or ERROR
    return TRUE;                                  // TODO: handle error
}


/***************************************************/

void GSM_uart_transmit(unsigned char data)
{
    while (!( UCSR1A & (1<<UDRE1)));               // wait while register is free
    UDR1 = data;                                   // load data in the register
}

/***************************************************/

unsigned char GSM_uart_recieve()
{
    while(!(UCSR1A) & (1<<RXC1));                  // wait while data is being received
    return UDR1;                                   // return 8-bit data
}

/***************************************************/

uint8_t wait_for(char c, uint8_t value)
{
    set_time_out(value);
    while(GSM_uart_recieve() != c && !timer1.time_out);

    if(timer1.time_out)
    {
        return FALSE;
    }

    return TRUE;
}

/****************************************************/

void GSM_send_str(char* cmd)
{
    uint8_t i;

    for(i=0;i<strlen(cmd);i++)
    {
        GSM_uart_transmit(cmd[i]);
    }
}

/***************************************************/

uint8_t GSM_send_SMS(char* text, char* number)
{
    print(GSM,"%s=\"00%s\"\r",cmd_send_SMS,number);
    while(GSM_uart_recieve() != '>');
    print(GSM,"%s%c",text,0x1A);  // <Ctrl + Z> in ASCII

    return TRUE;    //TODO: handle error
}

/***************************************************/

void GSM_process_response()
{
    char* data = (char*)GSM_uart.buffer;

    if(strstr(data,"+CCLK"))
    {
        date_time(data, 1);
        PC_send_str(data);
    }
    else if(strstr(data,"+CMGF"))
    {
        PC_send_str(data);
    }
    else if(strstr(data,"+CTZU"))
    {
        PC_send_str(data);
    }
    else if(strstr(data,"+CMTI"))
    {
        message.slot = get_SMS_slot(data);
        read_slot(message.slot);
    }
    else if(strstr(data,"+CMGR"))
    {
        if(parse_SMS(data, &message))
        {
            new_msg_received = TRUE;
        }
    }
    else if(strstr(data,"+CMGL"))
    {
        PC_send_str(data);
    }
    else if(strstr(data,"OK"))
    {
        PC_send_str(data);
    }
    else if(strstr(data,"ERROR"))
    {
        PC_send_str(data);
    }
    else
    {
        PC_send_str(data);
    }

    GSM_uart.status = WAITING;
}

uint8_t get_SMS_slot(char* buffer)
{
    uint8_t i = 0;

    while(buffer[i] != ',')
    {
        i++;
    }

    return atoi(buffer + i + 1);
}

void read_slot(uint8_t slot)
{
    print(GSM,"%s=%d\r",cmd_read_SMS,slot);

    GSM_uart.occurrence = 3;
}

uint8_t parse_SMS(char* data, SMS_t* msg)
{
    uint8_t i = 0, k = 0, commas = 0;

    if(strstr(data, "REC UNREAD"))
    {
        while(data[i] != '\0')
        {
            if(commas == 1 && data[i] != '"')
            {
                msg->sender[k] = data[i+1];
                k++;
            }
            else if(commas == 3)
            {
                msg->sender[11] = '\0';
                parse_date_time(msg->date_str, msg->time_str, msg->zone_str, data+i, TRUE);
                process_date(&msg->time, msg->date_str);
                process_time(&msg->time, msg->time_str);
                process_zone(&msg->time, msg->zone_str);
                commas++;
            }
            else if(commas == 4 && data[i] == '\n')
            {
                strncpy(msg->text, data+i+1, 160);
                return TRUE;
            }

            if(data[i] == ',')
            {
                commas++;
            }

            i++;
        }
    }

    return FALSE;
}

/***************************************************/
//        Power On/Off M72FA module
/***************************************************/
void switch_GSM()
{
    PORTD &= ~(1<<GSM_ON_OFF);
    _delay_ms(1100);
    PORTD |= (1<<GSM_ON_OFF);
    _delay_ms(1500);
    dev_stat.GSM_pwr = GSM_get_state();

    if(dev_stat.GSM_pwr)
    {
        config_GSM();
    }
}

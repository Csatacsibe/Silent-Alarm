#include "board.h"

volatile interrupt_flags_t int_flags;
volatile device_status_t   dev_stat;
volatile timer_variables_t timer1;
volatile uart_rx_t         GSM_uart;
SMS_t                      message;
date_t                     date;
uint8_t                    new_msg_received = FALSE;

ISR(ALARM_BUTT_ISR)
{
    int_flags.alarm_button = TRUE;
}

ISR(ALARM_SW_ISR)
{
    int_flags.alarm_sw = TRUE;
}

ISR(GSM_RI_ISR)
{
    int_flags.GSM = TRUE;
}

ISR(TIMER1_OVF_vect)
{
    timer1.ovf_cnt++;

    if(timer1.ovf_cnt == timer1.cnt_max)
    {
        TIMSK &= ~(1<<TOIE1);
        timer1.time_out = TRUE;
    }
}

ISR(TIMER0_COMP_vect)
{
    poll_inputs();
}

ISR(USART1_RX_vect)
{
    unsigned char data = UDR1;

    if(data == GSM_uart.begin && GSM_uart.status == WAITING)
    {
        GSM_uart.index  = 0;
        GSM_uart.status = STARTED;
    }
    else if(data == GSM_uart.end && GSM_uart.status == STARTED)
    {
        GSM_uart.occured++;
        if(GSM_uart.occured == GSM_uart.occurrence)
        {
            GSM_uart.buffer[GSM_uart.index] = '\0';
            GSM_uart.occured    = 0;
            GSM_uart.occurrence = 1;
            GSM_uart.status     = DONE;
            GSM_process_response();
        }
    }
    else if (GSM_uart.status == STARTED)
    {
        if(GSM_uart.index < UART_BUFFER_SIZE - 1)
        {
            GSM_uart.buffer[GSM_uart.index++] = data;
        }
        else
        {
            GSM_uart.status = BUFFER_FULL;
            PC_send_str("GSM buffer is full!");
            GSM_uart.status = WAITING;
        }
    }
}

void gsm_ri_handler()
{
	new_msg_received = FALSE;    
}

void alarm_button_handler()
{
    set_alarm_status(OFF);
    switch_LED(LED2, OFF);
}

void button1_handler()
{
    switch_LCD();
}

void button2_handler()
{
    set_alarm_enabled(is_alarm_enabled() ? FALSE : TRUE);
    display_alarm_enabled();
}

void sensor1_on_handler()
{
    if (FALSE == get_door_state())
    {
        set_door_state(TRUE);
    }
    display_door_state();
    switch_buzzer(OFF);
}

void sensor1_off_handler()
{
    if (TRUE == get_door_state())
    {
        set_door_state(FALSE);
    }
    display_door_state();
    switch_buzzer(OFF);
}

int main(void)
{
    init_board();
    init_LCD();
    init_GSM();
    
    sei();
    init_global_variables();
    init_alarm_handler();

    register_input(&PING, BUTT1, 1, 150, button1_handler);
    register_input(&PING, BUTT2, 1, 150, button2_handler);
    register_input(&PINC, HEADER7, 1, 400, sensor1_on_handler);
    register_input(&PINC, HEADER7, 0, 150, sensor1_off_handler);

    switch_LED(LED1, ON);
    switch_LCD();

    display_alarm_enabled();

    while(1)
    {
        if(PINE & (1<<SW1))
        {
            print(GSM,"%s?\r",cmd_get_time);
        }

        if(PINE & (1<<SW2))
        {

        }

        if (TRUE == get_door_state() && TRUE == is_alarm_enabled())
        {
            set_alarm_status(ON);
            switch_LED(LED2, ON);
        } 

        interrupt_handler((uint8_t*)&int_flags.alarm_button, alarm_button_handler);
        interrupt_handler((uint8_t*)&int_flags.alarm_sw, switch_GSM);
        interrupt_handler((uint8_t*)&int_flags.GSM, gsm_ri_handler);
        handle_inputs();
    }
    return 0;
}

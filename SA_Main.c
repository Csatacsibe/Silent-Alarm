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
	int_flags.butt = TRUE;
}

ISR(ALARM_SW_ISR)
{
	int_flags.sw = TRUE;
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
    dev_stat.milis++;
    PORTC ^= (1<<HEADER0);
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
			GSM_uart.buffer[GSM_uart.index]   = '\0';
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

ISR(SPI_STC_vect)
{
    // Code to execute
    // whenever transmission/reception
    // is complete.
}

void wrap_function()
{
	switch_LED(LED2, TOGGLE);
}

void wrap_function2()
{
	new_msg_received = FALSE;
}

int main(void)
{
	init_board();
	init_LCD();
	init_GSM();
    //init_RF();

	sei();
	init_global_variables();

	switch_LCD();
	config_GSM();
	switch_LED(LED1, ON);

	while(1)
	{
	    set_DDRAM_address(LCD_row_1);
	    print(LCD,"%d",dev_stat.milis);

		if(button(BUTT1))
		{
			print(GSM,"%s = %d + %d","three",1,2);
			_delay_ms(10);
			print(PC,"%s + %d","test",1);
			_delay_ms(10);
			print(LCD,"%d %d %d",1992,6,1);
		}

		if(button(BUTT2))
		{

		}

		if(PINE & (1<<SW1))
		{
			print(GSM,"%s?\r",cmd_get_time);
		}

		if(PINE & (1<<SW2))
		{
			GSM_send_str(cmd_del_all_SMS);
			wait_for('K', 36);
			GSM_send_str(cmd_list_SMS);
		}

		if(new_msg_received == TRUE)
		{
			LCD_clear_display();
			LCD_print_str("From: ");
			set_DDRAM_address(LCD_row_2 + 4);
			LCD_print_str("+");
			LCD_print_str(message.sender);
			_delay_ms(2000);

			LCD_clear_display();
			LCD_print_str("At: ");
			set_DDRAM_address(LCD_row_1 + 4);
			LCD_print_str(message.date_str);
			set_DDRAM_address(LCD_row_2 + 8);
			LCD_print_str(message.time_str);
			_delay_ms(2000);

			LCD_clear_display();
			LCD_print_str(message.text);
			_delay_ms(2000);
		}

		interrupt_handler((uint8_t*)&int_flags.butt, wrap_function2);
		interrupt_handler((uint8_t*)&int_flags.sw, switch_GSM);
		interrupt_handler((uint8_t*)&int_flags.GSM, wrap_function);

	}
	return 0;
}

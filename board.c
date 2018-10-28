#include "board.h"

static void init_timer0();
static void init_PC_uart();
static void PC_uart_transmit(unsigned char data);
static unsigned char PC_uart_recieve();
static void vprint(periphery_t target, const char *fmt, va_list argp);

void init_board()
{
    OSCCAL = CLK_8MHZ;

    DDRF |= (1<<LED1) | (1<<LED2);
    DDRE &= ~(1<<SW1) & ~(1<<SW2);
    DDRG &= ~(1<<BUTT1) & ~(1<<BUTT2);
    DDRB |= (1<<BUZZER);
    DDRC |= (1<<HEADER0) | (1<<HEADER1);

    TCCR1B |=(1<<CS10);               // time-out timer, no prescale

    EIMSK |= (1<<INT6) | (1<<INT5) | (1<<INT4);
    EICRB |= (1<<ISC40);              // INT on any logic level change, SW
    EICRB |= (1<<ISC51);              // INT on falling edge, GSM
    EICRB |= (1<<ISC61) | (1<<ISC60); // INT on rising edge, BUTTON

    init_PC_uart();
    init_timer0();
}

static void init_PC_uart()
{
    UBRR0H = (BAUDRATE>>8);                      // shift the register right by 8 bits
    UBRR0L = BAUDRATE;                           // set baud rate
    UCSR0B |= (1<<TXEN0)|(1<<RXEN0);             // enable receiver and transmitter
    UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01);           // 8bit data format
}

static void PC_uart_transmit(unsigned char data)
{
    while (!( UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

static unsigned char PC_uart_recieve()
{
    while(!(UCSR0A) & (1<<RXC0));
    return UDR0;
}

void PC_send_str(char* cmd)
{
    uint8_t i;

    for(i=0;i<strlen(cmd);i++)
    {
        PC_uart_transmit(cmd[i]);
    }
}

//***************************************************/
// Timer delay function, value = 1 --> 65.535ms delay
//***************************************************/
void set_time_out(uint8_t value)
{
    timer1.cnt_max   = value;                           // set time out limit
    timer1.ovf_cnt   = 0;
    timer1.time_out  = FALSE;
    TCNT1 = 0;
    TIMSK |= (1<<TOIE1);
}

static void init_timer0()
{
    // Timer Mode 4: Clear Timer on Compare match (CTC)
    TCCR0 |= (1<<WGM01);
    // Initialize Timer staring value
    TCNT0 = 0;
    // Set Compare value for 1ms overflow
    OCR0 = 250;
    // Enable Timer compare match interrupt
    TIMSK |= (1<<OCIE0);
    // Start Timer & Clock Select: Prescale I/O clock by 32
    TCCR0 |= (1<<CS01) | (1<<CS00);
}

///////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////

void init_global_variables()
{
    timer1.ovf_cnt  = 0;
    timer1.cnt_max  = 0;
    timer1.time_out = FALSE;

    button1.counter   = 0;
    button1.pin       = BUTT1;
    button1.port      = &PING;
    button1.is_active = &(int_flags.butt1);

    button2.counter   = 0;
    button2.pin       = BUTT2;
    button2.port      = &PING;
    button2.is_active = &(int_flags.butt2);

    int_flags.butt   = FALSE;
    int_flags.sw     = FALSE;
    int_flags.GSM    = FALSE;
    int_flags.timeout = FALSE;
    int_flags.butt1   = FALSE;
    int_flags.butt2   = FALSE;

    GSM_uart.buffer[0]  = '\0';
    GSM_uart.begin      = '\n';
    GSM_uart.end        = '\r';
    GSM_uart.index      = 0;
    GSM_uart.occurrence = 1;
    GSM_uart.occured    = 0;
    GSM_uart.status     = WAITING;

    dev_stat.LCD_pwr = LCD_get_state();
    dev_stat.GSM_pwr = GSM_get_state();
    dev_stat.GSM_configured = FALSE;
    dev_stat.LCD_configured = FALSE;
    dev_stat.is_date_displayed = FALSE;

    date.year  = 0;
    date.month = 0;
    date.day   = 0;
    date.hour  = 0;
    date.min   = 0;
    date.sec   = 0;
    date.zone  = 0;

    message.slot = 0;
}

void debounce_input(input_t* input)
{
    if (((*(input->port)) & (1<<input->pin))) 
    {
        input->counter++;
        if (100 == input->counter)
        {
            *(input->is_active) = TRUE;
            input->counter = 0;
        }
    }
    else
    {
        input->counter = 0;
    }
}

void switch_LED(char id, uint8_t state)
{
    if(state == ON)
    {
        PORTF |= (1<<id);
    }
    else if(state == TOGGLE)
    {
        PORTF ^= (1<<id);
    }
    else
    {
        PORTF &= ~(1<<id);
    }
}

void switch_buzzer(uint8_t state)
{
    if(state == ON)
    {
        PORTB |= (1<<BUZZER);
    }
    else if(state == TOGGLE)
    {
        PORTB ^= (1<<BUZZER);
    }
    else
    {
        PORTB &= ~(1<<BUZZER);
    }
}

void interrupt_handler(uint8_t* flag, void (*handler_func)(void))
{
    if(*flag)
    {
        *flag = FALSE;
        handler_func();
    }
}

static void vprint(periphery_t target, const char *fmt, va_list argp)
{
    char string[200];
    if(0 < vsprintf(string,fmt,argp))
    {
        switch(target)
        {
            case PC: PC_send_str(string);
                break;
            case GSM: GSM_send_str(string);
                break;
            case LCD: LCD_print_str(string);
                break;
            default:
                break;
        }
    }
}

void print(periphery_t target, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vprint(target, fmt, argp);
    va_end(argp);
}


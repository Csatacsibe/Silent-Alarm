/*uint8_t debounce(char button)
{
     uint16_t pressed_confidence_lvl = 0;
    uint16_t released_confidence_lvl = 0;
    
    while(1)
    {
        if((PING & (1<<button)))
        {
            pressed_confidence_lvl++;
            if(pressed_confidence_lvl > 4500)
            {
                return TRUE;
            }
        }
        else
        {
            released_confidence_lvl++;
            if(released_confidence_lvl > 4500)
            {
                return FALSE;
            }
        }
    }
}*/


// Timer1 initializtion
/*void init_timer1() 
{
    // Timer Mode 4: Clear Timer on Compare match (CTC)
    TCCR1B |= (1<<WGM12); 
    // Initialize Timer staring value
    TCNT1 = 0;
    // Set Compare value for 1s overflow
    OCR1A = 31250;
    // Enable Timer overflow interrupt
    TIMSK |= (1<<OCIE1A);
    // Start Timer & Clock Select: Prescale I/O clock by 256
    TCCR1B |= (1<<CS12); 
}*/

/* ISR(TIMER1_COMPA_vect)
{
    if(sec_cnt < 5)
    {
        PORTC |= (1<<HEADER1);  // turn on LED in first 5 sec
    }
    else
    {
        PORTC &= ~(1<<HEADER1);
    }

    sec_cnt++;
    PORTC ^= (1<<HEADER0);
    
    if(sec_cnt == 30)    // 12*60*60 = 43200
    {
        sec_cnt = 0;        // restart counting
    }
} */

uint8_t GSM_get_str(char start, char end, uint8_t value)
{
    GSM.begin   = start;
    GSM.end     = end;
    GSM.status  = WAITING;
            
    set_time_out(value);
    while(GSM.status != DONE  
       && GSM.status != BUFFER_FULL
       && !timer1.time_out);
    
    if(timer1.time_out)
    {
        return FALSE;
    }
    else if(GSM.status == BUFFER_FULL)
    {
        switch_LED(LED2, ON);
        return FALSE;
    }
    
    return TRUE;
}

/***************************************************/

uint8_t GSM_set_alarm(uint8_t state, char* date, uint8_t repeat, uint8_t power)
{
    char param[40];
    
    GSM_send_str(cmd_alarm);
    sprintf(param,"%d,",state);
    GSM_send_str(param);

    GSM_send_str(date);
    if(date_n_time.zone > -1)
    {
        sprintf(param,"+%.2d\",",date_n_time.zone);
        LCD_print_int(date_n_time.zone);
        GSM_send_str(param);
    }
    else
    {
        sprintf(param,"%.2d\",",date_n_time.zone);
        GSM_send_str(param);
    }
    sprintf(param,"%d,",repeat);
    GSM_send_str(param);    
    sprintf(param,"%d",power);
    GSM_send_str(param);
    
    return TRUE;
}


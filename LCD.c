#include "board.h"

uint8_t LCD_get_state()
{
    return !(PORTB & (1<<LCD_ON_OFF));
}

void init_LCD()
{
    DDRF |= (1<<RS) | (1<<En);
    DDRA = 0xFF;
    DDRB |= (1<<LCD_ON_OFF);
    PORTB |= (1<<LCD_ON_OFF);
    PORTF &= ~(1<<RS) & ~(1<<En);
}

void switch_LCD()
{
    PORTB ^= (1<<LCD_ON_OFF);
    dev_stat.LCD_pwr = LCD_get_state();
    _delay_ms(1500);

    if(dev_stat.LCD_pwr)
    {
        LCD_config();
    }
}

void LCD_config()
{
    function_set(1,1,0);    //DL = 1 for 8-bit interface, N = 1 to set the fonts for 2 line display and if N = 1 then value of F doesn't matter

    LCD_clear_display();

    display_control(1,0,0); //D = 1, so that display is On, C = 0, so that cursor is Off, B = 0 so that cursor is not blinking
    Entry_mode_set(1,0);    // I_D = 1 so that the cursor position should increment after each character display. S = 0 because we don't want the display to shift after each character display

    _delay_ms(50);
}

///////////////////////////////////////////////////////////////

void LCD_cmd(unsigned char cmd)
{
    PORTF &= ~(1<<RS);
    _delay_us(200);
    PORTF |= (1<<En);
    _delay_us(200);
    LCD_DATA= cmd;
    _delay_us(200);
    PORTF &= ~(1<<En);
    _delay_us(200);
    LCD_DATA= 0;
    _delay_us(200);
    return;
}

void LCD_write(unsigned char data)
{
    PORTF |= (1<<RS);
    _delay_us(200);
    PORTF |=(1<<En);
    _delay_us(200);
    LCD_DATA= data;
    _delay_us(200);
    PORTF &= ~(1<<En);
    _delay_us(200);
    LCD_DATA= 0;
    _delay_us(200);
    return ;
}

// The functions after this comment are the different commands for LCD

void LCD_clear_display() // Clears display and returns cursor to the home position (address 0)
{
    LCD_cmd(0x01); // clear
    LCD_cmd(0x02); // home
}

void LCD_cursor_home() // Returns cursor to home position (address 0). Also returns display being shifted to the original position. DDRAM contents remains unchanged.
{
    LCD_cmd(0x02);
}

void Entry_mode_set(unsigned char I_D, unsigned char S) // Sets cursor move direction (I/D), specifies to shift the display (S). These operations are performed during data read/write.
{
    /*
    I_D should be either 0 or 1
    0 - Decrement Position Cursor
    1 - Incrememt Position Cursor
    */
    /*
    S is to decide Shift
    0 - No Display Shift
    1 - Display Shift
    */
    unsigned char entry = (1<<2) | (1<<I_D) | (S); // formula to generate command byte
    LCD_cmd(entry);
}

void cursor_display_shift(unsigned char SC, unsigned char RL)
{
    /*
     SC  RL
     0   0   Shifts the cursor position to the left
             (Address Counter is decremented by 1)
     0   1   Shifts the cursor position to the right
             (Address Counter is incremented by 1)
     1   0   Shifts the entire display to the left
             The cursor follows the display shift
     1   1   Shifts the entire display to the right
             The cursor follows the display shift
    */
    unsigned char entry = (1<<4) | (SC<<3) | (RL<<2); // formula to generate command byte
    LCD_cmd(entry);
}

void display_control(unsigned char D, unsigned char C, unsigned char B) // Sets On/Off of all display (D), cursor On/Off (C) and blink of cursor position character (B).
{
    /*
    D is Display bit
    0 - Display off
    1 - Display on
    */
    /*
    C is cursor bit
    0 - Cursor off
    1 - Cursor on
    */
    /*
    B is to set whether the cursor will blink or not
    0 - Blink off
    1 - Blink on
    */
    unsigned char display = (1<<3) | (D<<2) | (C<<1) | (B); // formula to generate command byte
    LCD_cmd(display);
}


void function_set(unsigned char DL,unsigned char N,unsigned char F) // Sets interface data length (DL), number of display line (N) and character font(F).
{
    /*
    DL to decide whether 4-bit display or 8-bit display
    0 - 4-bit interface
    1 - 8-bit interface
    */
    /*
    N is to set number of display line
    F is for font size

        display Character  Duty
        N F  lines    Font    Factor Remarks
        === ======= ========= ====== =======
        0 0    1    5x 7 dots  1/8    -
        0 1    1    5x10 dots  1/11   -
        1 *    2    5x 7 dots  1/16  Cannot display 2 lines with 5x10 dot character font
    */
    // when N = 1 then two lines of display
    // when N = 0
    unsigned char func = (1<<5) | (DL<<4) | (N<<3) | (F<<2);
    LCD_cmd(func);
}


void set_DDRAM_address(unsigned char address) // Sets the DDRAM address. DDRAM data is sent or received after this setting.
{
    unsigned char display_address = (1<<7) | address; // formula for calculating the command byte
    // this command is to set the cursor on the inputted address

    LCD_cmd(display_address);
}


void LCD_print_str(char* word)
{
    unsigned char i=0;                                  // iterating variable
    unsigned char length = (unsigned char)strlen(word); // length of the word

    // Printing the word
    for(i=0;i<length;i++)
    {
        LCD_write(word[i]);
    }
}

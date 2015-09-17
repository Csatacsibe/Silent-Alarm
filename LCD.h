#ifndef LCD_H
#define LCD_H

#include "board.h"

#define LCD_row_1       0x80
#define LCD_row_2		0xC0

/*         prototypes       */

void 	init_LCD(void);				//MCU side
void 	switch_LCD(void);
uint8_t LCD_get_state();

/*         library          */

void LCD_config(void);				// LCD side init

void LCD_cmd(unsigned char cmd);			// Function to send command to the LCD
void LCD_write(unsigned char data);			// Function to display character on LCD

void LCD_clear_display(void);				// Function that clears the display and sets the cursor to home position
void LCD_cursor_home(void);					// Function that resets the position of cursor to home position

void set_DDRAM_address(unsigned char address);							// Sets the position of cursor to the given address
void Entry_mode_set(unsigned char I_D, unsigned char S);				// Function to set the properties of the cursor
void display_control(unsigned char D, unsigned char C, unsigned char B);// Function to set display (On/Off), Cursor (On/Off) and blinking (On/Off)
void function_set(unsigned char DL,unsigned char N,unsigned char F);	// Function to set the number of display lines and the font size
void cursor_display_shift(unsigned char SC, unsigned char RL);

void LCD_print_str(char* word);			// Function that prints the whole string on the LCD

void LCD_print_str_xy(uint8_t x, uint8_t y, char* word);

#endif
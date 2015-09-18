#ifndef TIME_H
#define TIME_H

#include "board.h"
#include "data_types.h"

/*     prototypes      */

void    date_time(char* data, uint8_t mode);
void    parse_date_time(char* date_t, char* time, char* zone, char* str, uint8_t format);
uint8_t process_date(date_t* s_date, char* date_t);
uint8_t process_time(date_t* s_date, char* time);
uint8_t process_zone(date_t* s_date, char* zone);
void    refresh_date_display(date_t* prev, date_t* current);
void    refresh_time_display(date_t* prev, date_t* current);
void    init_date_display(date_t* current);

#endif

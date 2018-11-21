#ifndef ALARM_H
#define ALARM_H

void init_alarm_handler(void);
uint8_t is_alarm_enabled(void);
void set_alarm_enabled(uint8_t enabled);
uint8_t get_alarm_status(void);
void set_alarm_status(uint8_t status);
uint8_t get_door_state(void);
void set_door_state(uint8_t state);
void display_alarm_enabled(void);
void display_door_state(void);

#endif
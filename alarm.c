#include "board.h"

typedef struct alarm_handler
{
    uint8_t enabled;
    uint8_t status;
    uint8_t door_state;
}alarm_handler_t;

static volatile alarm_handler_t handler;

void init_alarm_handler(void)
{
    handler.enabled = FALSE;
    handler.status = OFF;
    handler.door_state = FALSE;
}

uint8_t is_alarm_enabled(void)
{
    return handler.enabled;
}

void set_alarm_enabled(uint8_t enabled)
{
    if (TRUE == enabled && TRUE == handler.door_state)
    {
        return;
    }
    handler.enabled = enabled;
}

uint8_t get_alarm_status(void)
{
    return handler.status;
}

void set_alarm_status(uint8_t status)
{
    handler.status = status;
}

uint8_t get_door_state(void)
{
    return handler.door_state;
}

void set_door_state(uint8_t state)
{
    handler.door_state = state;
}

void display_alarm_enabled(void)
{
    LCD_clear_display();
    set_DDRAM_address(LCD_row_1);
    if (FALSE == handler.enabled) {
        print(LCD,"  KIKAPCSOLVA");
    }
    else
    {
        print(LCD,"  BEKAPCSOLVA");
    }
}

void display_door_state(void)
{
    set_DDRAM_address(LCD_row_2);
    if (TRUE == handler.door_state) {
        print(LCD,"  AJTO NYITVA");
    }
    else
    {
        print(LCD,"             ");
    }
}

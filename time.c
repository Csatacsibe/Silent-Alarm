#include "board.h"

void parse_date_time(char* date, char* time, char* zone, char* str, uint8_t format)
{
	uint8_t i,j = 0, dt = FALSE, tm = FALSE, start = FALSE;
	for(i=0;i<strlen(str);i++)
	{
		if(start)
		{
			if(!dt)
			{
				if(str[i] == ',')
				{
					date[j] = '\0';
					dt = TRUE;
					j = 0;
				}
				else if(format && str[i] == ' ')
				{
					date[j] = '\0';
					dt = TRUE;
					j = 0;
				}
				else
				{
					date[j] = str[i];
					j++;
				}
			}
			else
			{
				if(!tm)
				{
					if(str[i] == '+' || str[i] == '-')
					{
						time[j] = '\0';
						tm = TRUE;
						zone[0] = str[i];
						j = 1;
					}
					else
					{
						time[j] = str[i];
						j++;
					}
				}
				else
				{
					if(str[i] == '"')
					{
						zone[j] = '\0';
						return;
					}
					else
					{
						zone[j] = str[i];
						j++;
					}
				}
			}
		}

        if (format && i == 2)
        {
            start = TRUE;
        }
		else if(!format && str[i] == '"')
		{
			start = TRUE;
		}
	}
}

uint8_t process_date(date_t* s_date, char* tmp_date)
{
	if(strlen(tmp_date) != 8) return FALSE;

    char temp[3];
    uint8_t i,j=0;
    for(i=0;i<8;i++)
    {
        if(tmp_date[i] != '/')
        {
            temp[j] = tmp_date[i];
            j++;
        }

		switch(i)
		{
			case 1: temp[2] = '\0';
					s_date->year = atoi(temp);
					j=0;
				break;
			case 4:	temp[2] = '\0';
					s_date->month = atoi(temp);
					j=0;
				break;
			case 7: temp[2] = '\0';
					s_date->day = atoi(temp);
					j=0;
				break;
		}
    }
	return TRUE;
}

uint8_t process_time(date_t* s_date, char* time)
{
	if(strlen(time) != 8) return FALSE;

    char temp[3];
    uint8_t i,j=0;
    for(i=0;i<8;i++)
    {
        if(time[i] != ':')
        {
            temp[j] = time[i];
            j++;
        }

		switch(i)
		{
			case 1: temp[2] = '\0';
					s_date->hour = atoi(temp);
					j=0;
				break;
			case 4:	temp[2] = '\0';
					s_date->min = atoi(temp);
					j=0;
				break;
			case 7: temp[2] = '\0';
					s_date->sec = atoi(temp);
					j=0;
				break;
		}
    }
	return TRUE;
}

uint8_t process_zone(date_t* s_date, char* zone)
{
	if(strlen(zone) != 3) return FALSE;

	s_date->zone = atoi(zone);
	return TRUE;
}

void refresh_date_display(date_t* prev, date_t* current)
{
    if(current->day != prev->day)
    {
		set_DDRAM_address(LCD_row_1 + 6);
		print(LCD,"%.2d",current->day);
    }

    if(current->month != prev->month)
    {
		set_DDRAM_address(LCD_row_1 + 3);
		print(LCD,"%.2d",current->month);
    }

    if(current->year != prev->year)
    {
		set_DDRAM_address(LCD_row_1 + 0);
		print(LCD,"%.2d",current->year);
    }
}

void refresh_time_display(date_t* prev, date_t* current)
{
    if(current->sec != prev->sec)
    {
		set_DDRAM_address(LCD_row_2 + 6);
		print(LCD,"%.2d",current->sec);
    }

    if(current->min != prev->min)
    {
		set_DDRAM_address(LCD_row_2 + 3);
		print(LCD,"%.2d",current->min);
    }

    if(current->hour != prev->hour)
    {
		set_DDRAM_address(LCD_row_2 + 0);
		print(LCD,"%.2d",current->hour);
    }
}

void init_date_display(date_t* current)
{
	LCD_clear_display();
	set_DDRAM_address(LCD_row_1 + 2);
	print(LCD,"/");
	set_DDRAM_address(LCD_row_1 + 5);
	print(LCD,"/");
	set_DDRAM_address(LCD_row_2 + 2);
	print(LCD,":");
	set_DDRAM_address(LCD_row_2 + 5);
	print(LCD,":");

	set_DDRAM_address(LCD_row_2 + 6);
	print(LCD,"%.2d",current->sec);
	set_DDRAM_address(LCD_row_2 + 3);
	print(LCD,"%.2d",current->min);
	set_DDRAM_address(LCD_row_2 + 0);
	print(LCD,"%.2d",current->hour);
	set_DDRAM_address(LCD_row_1 + 6);
	print(LCD,"%.2d",current->day);
	set_DDRAM_address(LCD_row_1 + 3);
	print(LCD,"%.2d",current->month);
	set_DDRAM_address(LCD_row_1 + 0);
	print(LCD,"%.2d",current->year);

	dev_stat.is_date_displayed = TRUE;
}

/********************************************/
// mode = 1, get date/time & display on LCD
// mode = 0, get date/time
/********************************************/
void date_time(char* data, uint8_t mode)
{
	date_t prev = date;
	char tmp_date[12], tmp_time[12], tmp_zone[6];

	parse_date_time(tmp_date,tmp_time,tmp_zone,data, FALSE);

	if(mode)
	{
		if(dev_stat.is_date_displayed == FALSE)	init_date_display(&date);
	}
	else
	{
		dev_stat.is_date_displayed = FALSE;
	}

	if(process_time(&date,tmp_time) && mode)
	{
		refresh_time_display(&prev, &date);
	}

	if(process_date(&date,tmp_date) && mode)
	{
		refresh_date_display(&prev, &date);
	}

	process_zone(&date,tmp_zone);

}


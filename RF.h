#ifndef RF_H
#define RF_H

void init_RF(void);
void init_RF_SPI();

unsigned char spi_tranceiver(unsigned char data);

#endif
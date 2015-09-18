#include "board.h"

void init_RF()
{
    DDRB |= (1<<RF_CS) | (1<<RF_CE);
    init_RF_SPI();
}

void init_RF_SPI()
{
    DDRB |= (1<<SPI_SS) | (1<<SPI_CLK) | (1<<SPI_MOSI);   //SS pin as output

    SPCR |= (1<<MSTR) | (1<<SPR0) | (1<<SPE);// | (1<<SPIE);
    
    SPSR |= (1<<SPI2X);
}

unsigned char spi_tranceiver(unsigned char data)
{
    // Load data into the buffer
    SPDR = data;
 
    //Wait until transmission complete
    while(!(SPSR & (1<<SPIF) ));
 
    // Return received data
    return(SPDR);
}

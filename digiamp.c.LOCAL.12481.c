/*
 * digiamp.c
 *
 * Created: 09.09.2013 21:10:27
 *  Author: transistorgrab
 */ 


#include <avr/io.h>
#include <avr/iomx8.h>
#include "digiamp.h"	/** defines for the project	*/




/** \brief: initiates all controller pins for input or output


*/
void init_ports(void)
{
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;

	MUX_SOURCE_SEL_0 = 0;		/** select MUX source 0		*/
	MUX_SOURCE_SEL_1 = 0;
	
	AMP_ENABLE = 1;				/** unmute power amplifier	*/
	
	MUX_AUDIO_OFF = 0;			/** unmute MUX				*/
	
	SPI_CLK = 0;				/** initiate Soft-SPI pins	*/
	SPI_DAT = 0;
	SPI_CS1_ATT = 1;
	SPI_CS2_LED = 1;
}

/** init the timer for the scheduler cycles	*/
void init_timer(void)
{
	
}

int main(void)
{
	init_ports();
	init_timer();
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

/** 
 * digiamp.c
 *
 * Created: 09.09.2013 21:10:27
 *  Author: transistorgrab
 */ 


#include <avr/io.h>
#include <avr/iomx8.h>
#include "digiamp.h"	/** defines for the project	*/



/** this function loads the settings for volume and source from the eeprom
 * and restores the settings. sets volume and source to the last saved state	*/
void restore_settings(void)
{
	uint8_t temp;
	if (temp = eeprom_read_byte(&ee_source)) /** yes, thats right, that is a "="	*/
		set_source(temp);	/* if there is a value stored, restore it	*/
	else
		set_source(1);		/* if there is no value stored, set source to 1	*/

	set_volume(recall_volume(1),recall_volume(0));	/* first start may be very loud...	*/
}

/** \brief: initiates all controller pins for input or output
*/
void init_ports(void)
{
	restore_settings();	/** restore source and volume settings	*/

	AMP_ENABLE = 1;				/** unmute power amplifier	*/
	
	SPI_CLK = 0;				/** initiate Soft-SPI pins	*/
	SPI_DAT = 0;
	SPI_CS1_ATT = 1;
	SPI_CS2_LED = 1;

	DDRB = DDRB_SETTING;        /** set direction settings on ports */
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
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

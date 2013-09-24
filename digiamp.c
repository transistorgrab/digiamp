/** 
 * digiamp.c
 *
 * Created: 09.09.2013 21:10:27
 *  Author: transistorgrab
 */ 
#ifndef F_CPU
#define F_CPU 1000000UL //factory default clock value
#endif

#include <avr/io.h>
#include <avr/iomx8.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "digiamp.h"	/** defines for the project	*/

/** this function loads the settings for volume and source from the eeprom
 * and restores the settings. sets volume and source to the last saved state	*/
void restore_settings(void)
{
	AMP_ENABLE = 0;	/** mute power amplifier	*/

	uint8_t temp, temp1;
	temp = recall_source();
	if (temp == 255)	/** default value from empty eeprom	*/
		temp = SOURCE_MIN;	/** set useful value	*/

	if (temp)	/** is there a source value	*/
	{
		set_source(temp);	/** if there is a value stored, restore it	*/
		get_source(temp);	/** send restored value to the source switching function	*/
	}
	else
	{
		set_source(1);		/** if there is no value stored, set source to 1	*/
		get_source(temp);	/** send restored value to the source switching function	*/
	}

	temp=recall_volume(1);	/** used twice	*/
	if (temp == 255)	/** default value from empty eeprom	*/
		temp = VOLUME_MIN;	/** set useful value	*/
	temp1=recall_volume(0);	/** recall value for left volume	*/
	if (temp1 == 255)	/** default value from empty eeprom	*/
		temp1 = VOLUME_MIN;	/** set useful value	*/
	set_volume(temp,temp1);	/** set recalled volume value			*/
	get_volume(0,temp);		/** send volume setting to volume changing function	*/

	AMP_ENABLE = 1;	/** unmute power amplifier	*/
}

/** \brief: initiates all controller pins for input or output
*/
void init_ports(void)
{

	AMP_ENABLE = 0;				/** mute power amplifier	*/
	
	SPI_CLK = 0;				/** initiate Soft-SPI pins	*/
	SPI_DAT = 0;
	SPI_CS1_ATT = 1;
	SPI_CS2_LED = 1;

	DDRB = DDRB_SETTING;        /** set direction settings on ports */
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	
//	EIFR  |= 0x01;	/** activate execution of interrupt 0	*/
	EIMSK |= 0x01;	/** activate external interrupt 0	*/
	EICRA |= 0x03;	/** set external interrupt 0 at rising edge	*/
//	VOL_B_PU = 1;	/** activate pullup for Port D.2	*/
}

/** init the timer for the scheduler cycles
	the timer is set for a 10 ms cycle, so everything is done on a 100 Hz basis	*/
void init_timer(void)
{
	TCCR0A |= 0x02;	/** set CTC mode	*/
	TCCR0B |= 0x04;	/** set prescaler to 256	*/
	OCR0A  = 39;		/** at 1 MHz clock: (1000000 Hz)/ 256 / (100 Hz) = 39 counter tics	*/
	TIMSK0 |= 0x02;	/** activate timer compare match A interrupt	*/
	
}

int main(void)
{
	init_ports();
	init_timer();
	restore_settings();	/** restore source and volume settings, must be done after port initialisation	*/

	
	sei();	/** global enabling of interrupts	*/
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

/** timer interrupt, do everything from catching input values to setting outputs	*/
ISR (TIMER0_COMPA_vect)
{
	sei();	/** enable global interrupts (volume setting)	*/
	uint8_t volume;	/** for the time being there is no balance setting, so volume right and left are the same	*/
	uint8_t source;
	static int8_t last_source;	/** prevent sound noise by setting source to the same value	*/

	source = get_source(0); /** which source should be set	*/
	if (source - last_source)	/** if both are equal do not call set_source	*/
	{
		set_source(source);		/** set MUX to source		*/
		last_source = source;	/** remember current source	*/
	}
	volume = get_volume(0,0);
	set_volume(volume, volume);
	/** save latest values to EEPROM	*/
	save_volume(volume, volume);
	save_source(source);
}

/** external interrupt 0 is bound to a incremental encoder,
	at every activation the volume will be set higher or lower
	according to the rotation direction:
		rotate left for lower volume
		rotate right for higher volume
		*/
ISR (INT0_vect) /** interrupt is activated at rising edge of VOL_B	*/
{
	if(VOL_A)			/** clock wise turn						*/
		get_volume(1,0);	/** increase volume						*/
	else				/** VOL_A is 0, counter clock wise turn	*/
		get_volume(-1,0); /** decrease volume						*/
}

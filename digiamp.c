/**
 * digiamp.c
 *
 * Created: 09.09.2013 21:10:27
 *  Author: transistorgrab
 */
#ifndef F_CPU
#define F_CPU 8000000UL //firmware set clock value
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

	uint8_t temp;//, temp1;
	temp = recall_source();
	if ((temp == 0xFF)||(temp<SOURCE_MIN)||(temp>SOURCE_MAX))	/** default value from empty eeprom or illegal value	*/
		temp = SOURCE_MIN;	/** set useful value	*/

	set_source(temp);	/** if there is a value stored, restore it	*/
	get_source(temp);	/** send restored value to the source switching function	*/

	temp = recall_volume(1);	/** used twice	*/
	if ((temp == 0xFF)||(temp<VOLUME_MAX)||(temp>VOLUME_MIN))	/** default value from empty eeprom	or illegal value	*/
		temp = VOLUME_MIN;	/** set useful value	*/

//	temp1=recall_volume(0);	/** recall value for left volume; currently not in use	*/
//	if (temp1 == 0xFF)	/** default value from empty eeprom	*/
//		temp1 = VOLUME_MIN;	/** set useful value	*/

	set_volume(temp,temp);	/** set recalled volume value			*/
	get_volume(0,temp);		/** send volume setting to volume changing function	*/

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

	EIMSK |= 0x01;	/** activate external interrupt 0	*/
	EICRA |= 0x03;	/** set external interrupt 0 at rising edge	*/
}

/** init the timer for the scheduler cycles
	the timer is set for a 10 ms cycle, so everything is done on a 100 Hz basis	*/
void init_timer(void)
{
	/** set core frequency to ~8MHz	*/
	cli();	/** disable all interupts	*/
	CLKPR = (1 << CLKPCE);	/** enable a change to CLKPR	*/
	CLKPR = 0x00;				/** set the CLKDIV to 1 - was 0011b = div by 8 taking 8MHz to 1MHz	*/

	TCCR0A |= 0x02;	/** set CTC mode	*/
//	TCCR0B |= 0x04;	/** set prescaler to 256	*/
//	OCR0A  =  39;		/** at 1 MHz clock: (1000000 Hz)/ 256 / (100 Hz) = 39 counter tics	*/
	TCCR0B |= 0x05;	/** set prescaler to 1024	*/
	OCR0A  =  194;		/** at 8 MHz clock: (8000000 Hz)/ (2 * 1024 * (20)) = (194 +1) counter tics	*/
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
	static uint8_t startup_delay=90;	/** switching the power amplifier on after full startup of power supply 90 = 1s@15,3 Hz*/
	static uint8_t runs=0;	/** divides call frequency for system updates	*/
	uint8_t volume;	/** for the time being there is no balance setting, so volume right and left are the same	*/
	uint8_t source;
	static uint8_t last_source, last_volume;	/** prevent sound noise caused by setting source to the same value	*/

	runs++;
	if (1) /** run input/output updates with 10 Hz update rate	*/
	{		
		source = get_source(0); /** which source should be set	*/
		if (!(source == last_source))	/** if both are equal do not call set_source	*/
		{
			set_source(source);		/** set MUX to source		*/
			last_source = source;	/** remember current source	*/
			save_source(source);
		}
		volume = get_volume(0,0);
		if (!(volume == last_volume))
		{
			set_volume(volume, volume);
			/** save latest values to EEPROM	*/
			last_volume = volume;
			save_volume(volume, volume);
		}
		runs=0;	/** reset run counter	*/
	}

	if (startup_delay)
	{
//		set_leds(0,0);
		startup_delay--;
//		if(!startup_delay%4)	/** do some fancy stuff with the LEDs during wait for power amp active **/
//		{
//			set_leds((startup_delay>>2)&0x03,((!startup_delay)>>2)&0x03);
//		}
		if(!startup_delay) /** startup delay timer expired	*/
		{
			AMP_ENABLE = 1;	/** unmute power amplifier	*/
			restore_settings();	/** restore source and volume settings, set LEDs according to actual settings	*/
		}

	}
}

/** external interrupt 0 is bound to an incremental encoder,
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

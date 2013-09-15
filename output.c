/** this file contains all functions for output operations	*/

#include "digiamp.h"

/** this function sets the LEDs via Soft SPI
    vector for Leds: 8 bit, lowes 4 = source(1..4), highest 4 = volume(5..8)
	volume is set with inverse values: 0 = highest volume, 0x7E = lowest volume
	volume must be converted to 4 bits according to volume,
	all volume leds will be set at highest volume, at lowest one led will be set	*/
void set_leds(uint8_t led, uint8_t volume)
{
	static uint8_t output_vector = 0x11; /** default: 1st source, lowest volume	*/
	uint8_t volume_calc;	/** temporay used variable for output calculation	*/
	if (!led)	/** volume leds will be set	*/
	{
		volume_calc = (0x7F & (0xFF ^ volume));	/** invert volume value for calculation and set MSB to 0	*/
		volume_calc /= 32;	/** highest value = 127, calculate how many leds to set (1..4)	*/
		switch (volume_calc)
			case 0:		/** volume is lower or equal 25%	*/
				output_vector |= 0x1F;
			break;  
		output_vector;
	}
	
}

/** this function sets the volume via Soft SPI	*/
void set_volume(int volume_right, int volume_left)
{
	
}
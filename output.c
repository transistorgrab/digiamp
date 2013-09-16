/** this file contains all functions for output operations	*/

#include "digiamp.h"

/** this function sets the LEDs via Soft SPI
    vector for Leds: 8 bit, lowes 4 = source(1..4), highest 4 = volume(5..8)
	volume is set with inverse values: 0 = highest volume, 0x7E = lowest volume
	volume must be converted to 4 bits according to volume,
	all volume leds will be set at highest volume, at lowest one led will be set
	if setting source LEDs volume parameter must be 0
	if setting volume LEDs source parameter must be 0*/
void set_leds(uint8_t source, uint8_t volume)
{
	static uint8_t output_vector = 0x11; /** default: 1st source, lowest volume	*/
	uint8_t volume_calc;	/** temporay used variable for output calculation	*/
	if (!source)	/** volume leds will be set, source values go from 1 to 4	*/
	{
		volume_calc = (0x7F & (0xFF ^ volume));	/** invert volume value for calculation and set MSB to 0	*/
		volume_calc /= 16;	/** highest value = 127, calculate how many leds to set (1..4)	*/
		switch (volume_calc)
			case 0:		/** volume is lower or equal 12.5%, set 1 LED			*/
				output_vector &= 0x0F;	/** delete all volume bits (LEDs off)	*/
				output_vector |= 0x10;	/** set volume LED 1					*/
			break;
			case 1:
			case 2:
			case 3:		/** volume is higher 12.5% and lower 50%				*/
				output_vector &= 0x0F;	/** delete all volume bits (LEDs off)	*/
				output_vector |= 0x30;	/** set volume LED 1 + 2				*/
			break;
			case 4:
			case 5:
			case 6:		/** volume is higher 50% and lower 87.5%				*/
				output_vector &= 0x0F;	/** delete all volume bits (LEDs off)	*/
				output_vector |= 0x70;	/** set volume LED 1 + 2 + 3			*/
			break;
			default:	/** volume is higher 87.5%				*/
				output_vector |= 0xF0;	/** set all volume LEDs	*/
	}
	
	spi_send(output_vector, 0, 0)
}

/** this function sets the volume via Soft SPI	*/
void set_volume(int volume_right, int volume_left)
{
	
}

/** this function sends the actual data via Soft SPI
	since there are only two SPI slaves no additional efford for more universal usage is done
	for LED SPI slave there are only 8 bit of data to be sent
	for volume control two 8 bit values (left and right) need to be sent
	so if the first parameter is 0 it is assumed, that volume data is to be sent
	if the first parameter is not 0 it is asumed that LED data is to be sent*/
void spi_send(uint8_t led, uint8_t volume_r, uint8_t volume_l)
{
	if (led)	/** LED data should be sent	*/
	{
		SPI_CS2_LED = 0;	/** acitvate chip select for LEDs		*/
		for (f=7;f==0;f--)	/** send all bits of the data MSB first	*/
		{
			if (led & 0x80)	/** if MSB is set then send 1 to the SPI data port		*/
				SPI_DAT = 1;
			else			/** if MSB is not set then send 0 to the SPI data port	*/
				SPI_DAT = 0;
			SPI_CLK = 1;	/** send rising edge to SPI clock port	*/
			led <<= 1;		/** shift data 1 to the left, new MSB value	*/
		}
		SPI_CS2_LED = 1;	/** deactivate chip select for LEDs	*/
	}
	else		/** send volume data	*/
	{
		
	}
	
}

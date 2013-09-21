/** this file contains all functions for output operations	*/

#include "digiamp.h"
#include <avr/eeprom.h>

/** this function sets the LEDs via Soft SPI
	vector for Leds: 8 bit, lowes 4 = source(1..4), highest 4 = volume(5..8)
	[v4,v3,v2,v1,s4,s3,s2,s1]

	volume is set with inverse values: 0 = highest volume, 0x7E = lowest volume
	volume must be converted to 4 bits according to volume,
	all volume LEDs will be set at highest volume, at lowest, one LED will be set
	if setting volume LEDs, source parameter must be 0, volume level can be 0	*/
void set_leds(uint8_t source, uint8_t volume)
{
	static uint8_t output_vector = 0x11; /** default: 1st source, lowest volume	*/
	uint8_t volume_calc;	/** temporay used variable for output calculation	*/
	if (!source)	/** volume leds will be set, source values go from 1 to 4	*/
	{
		volume_calc = (0x7F & (0xFF ^ volume));	/** invert volume value for calculation and set MSB to 0	*/
		volume_calc /= 16;	/** highest value = 127, calculate how many leds to set (1..4)	*/
		output_vector &= 0x0F;	/** delete all volume bits (LEDs off)	*/

		switch (volume_calc)
		{
			case 0:		/** volume is lower or equal 12.5%, set 1 LED			*/
				output_vector |= 0x10;	/** set volume LED 1					*/
				break;
			case 1:
			case 2:
			case 3:		/** volume is higher 12.5% and lower 50%				*/
				output_vector |= 0x30;	/** set volume LED 1 + 2				*/
				break;
			case 4:
			case 5:
			case 6:		/** volume is higher 50% and lower 87.5%				*/
				output_vector |= 0x70;	/** set volume LED 1 + 2 + 3			*/
				break;
			default:	/** volume is higher 87.5%				*/
				output_vector |= 0xF0;	/** set all volume LEDs	*/
		}
	}
	if (source)	/** set source leds	*/
	{
		output_vector &= 0xF0;	/** delete all source bits (source LEDs off)	*/

		switch (source)
		{
			case 1:
				output_vector |= 0x01;	/** set source LED 1			*/
				break;
			case 2:
				output_vector |= 0x02;	/** set source LED 4			*/
				break;
			case 3:
				output_vector |= 0x04;	/** set source LED 3			*/
				break;
			case 4:
				output_vector |= 0x08;	/** set source LED 4			*/
				break;
			default:
				output_vector |= 0x0F;	/** source value out of range, all LEDs on! alert!	*/
		}
	}
	
	spi_send(output_vector, 0, 0);
}

/** this function sets the volume via Soft SPI and triggers the volume LED output	*/
void set_volume(uint8_t volume_right, uint8_t volume_left)
{
	uint8_t volume;
	volume = (volume_right/2 + volume_left/2);	/** volume to display is the mean of r and l	*/
	
	spi_send(0, volume_right, volume_left);		/** send volume setting via spi to attenuator	*/
	
	set_leds (0,volume);	/** display new volume setting	*/
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
		SPI_CS2_LED = 0;	/** activate chip select for LEDs	*/
		spi_data_out(led);	/** send data to SPI port			*/
		SPI_CS2_LED = 1;	/** deactivate chip select for LEDs	*/
	}
	else		/** send volume data	*/
	{
		SPI_CS1_ATT = 0; 	/** activate chip select for attenuator				*/
		
		spi_data_out(0x00);	/** "magic number" select right channel				*/
		spi_data_out(volume_r);	/** send attenuator setting for right channel	*/

		SPI_CS1_ATT = 1; 	/** deactivate chip select for attenuator			*/
		SPI_CS1_ATT = 0; 	/** activate chip select for attenuator				*/
		
		spi_data_out(0x01);	/** "magic number" select left channel				*/
		spi_data_out(volume_l);	/** send attenuator setting for left channel	*/
		
		SPI_CS1_ATT = 1; 	/** deactivate chip select for attenuator			*/
	}
	
}

/** this function puts all data bits of the data parameter to SPI_DAT MSB first
 * and generates the SPI clock accordingly	*/
void spi_data_out(uint8_t data)
{
	uint8_t f;	/** loop variable	*/
	for (f=8;f!=0;f--)	/** send all bits of the data MSB first	*/
	{
		if (data & 0x80)	/** if MSB is set then send 1 to the SPI data port		*/
			SPI_DAT = 1;
		else				/** if MSB is not set then send 0 to the SPI data port	*/
			SPI_DAT = 0;
		SPI_CLK = 1;		/** send rising edge to SPI clock port	*/
		data <<= 1;			/** shift data 1 to the left, new MSB value	*/
		SPI_CLK = 0;		/** send falling edge to SPI clock port	*/
	}
}

/** save volume and source setting to eeprom	*/
void save_volume(uint8_t volume_r, uint8_t volume_l)
{
	eeprom_update_byte(&ee_volume_right,volume_r);
	eeprom_update_byte(&ee_volume_left,volume_l);
}

/** save volume and source setting to eeprom	*/
void save_source(uint8_t source)
{
	eeprom_update_byte(&ee_source,source);
}

/** recall volume and source settings from eeprom (after power up)	*/
uint8_t recall_volume (uint8_t right1_or_left0)
{
	if (right1_or_left0)	/** >0 => read right volume value from eeprom	*/
		return eeprom_read_byte(&ee_volume_right);
	else					/** 0 => read left volume value from eeprom	*/
		return eeprom_read_byte(&ee_volume_left);
}

uint8_t recall_source (void)
{
	return eeprom_read_byte(&ee_source);
}

/** set source for multiplexer
 * source values: 1..4 map to output values 0x00..0x03	*/
void set_source	(uint8_t source)
{
	set_leds(source,0);	/** set leds to display selected source	*/
	
	source--;	/** reduce source value for easier filtering	*/

	/** mute multiplexer before switching source	*/
	MUX_AUDIO_OFF = 1;
	
	if (0x01 & source)	/** filter for bit 0 */
		MUX_SOURCE_SEL_0 = 1;	/** if bit 0 is set, output 0 to line 0	*/ 
	else
		MUX_SOURCE_SEL_0 = 0;	/** if bit 0 is not set, output 0 to line 0 */
	if (0x02 & source)	/** filter for bit 1 */
		MUX_SOURCE_SEL_1 = 1;
	else
		MUX_SOURCE_SEL_1 = 0;

	/** unmute multiplexer before switching source	*/
	MUX_AUDIO_OFF = 0;
}

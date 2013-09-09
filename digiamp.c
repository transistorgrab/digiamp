/*
 * digiamp.c
 *
 * Created: 09.09.2013 21:10:27
 *  Author: transistorgrab
 */ 


#include <avr/io.h>
#include <avr/iomx8.h>

int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

/** \brief: initiates all controller pins for input or output


*/
void init_ports(void)
{
							/** Port B0 unused						*/
							/** Port B1 unused						*/
							/** Port B2 unused						*/
	DDRB = (1 << DDB3);		/** Port B3 input, AVR_MOSI				*/
	DDRB = (1 << DDB4);		/** Port B4 output, AVR_MISO			*/
	DDRB = (1 << DDB5);		/** Port B5 input, AVR_SCK				*/
	DDRB = (1 << DDB6);		/** Port B6 output, CS2 for LED			*/
	DDRB = (1 << DDB7);		/** Port B7 output, MUX_AUDIO_OFF		*/
	DDRC = (1 << DDC0);		/** Port C0 output, MUX_SOURCE_SEL_0	*/
	DDRC = (1 << DDC1);		/** Port C1 output, MUX_SOURCE_SEL_1	*/
	DDRC = (1 << DDC2);		/** Port C2 output, PA_MUTE				*/
	DDRC = (1 << DDC3);		/** Port C3 output, SCLK				*/
	DDRC = (1 << DDC4);		/** Port C4 output, CS1					*/
	DDRC = (1 << DDC5);		/** Port C5 output, SDAT				*/
	DDRC = (1 << DDC6);		/** Port C6 input, RESET				*/
							/** Port C7 not available				*/
							/** Port D0 unused						*/
	DDRD = (0 << DDD1);		/** Port D1 input, incremental input A	*/
	DDRD = (0 << DDD2);		/** Port D2 input, incremental input B	*/
	DDRD = (0 << DDD3);		/** Port D3 output, SDAT				*/
	DDRD = (0 << DDD4);		/** Port D4 output, SDAT				*/
							/** Port D5 not available				*/
							/** Port D6 not available				*/
							/** Port D7 not available				*/

	PORTC = (0 << PC0) | (0 << PC1);				/** select MUX source 0		*/
	PORTC = (1 << PC2);								/** unmute power amplifier	*/
	PORTB = (0 << PB7);								/** unmute MUX				*/
	
	PORTC = (0 << PC3) | (1 << PC4) | (0 << PC5);	/** initiate Soft-SPI pins	*/
	PORTB = (0 << PB6);								/** initiate Soft-SPI pins	*/
}

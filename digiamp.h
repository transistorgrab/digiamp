/** project file for all project specific defines and functions	*/
/** project uses ATMEGA88AU (http://www.atmel.com/images/doc2545.pdf)	*/

#include <stdint.h>	/** unsigned data types	*/


/** defines for ports **
**
** hardware pin configuration:
** all not defined ports are not used or not available	*/

/** struct and define for setting single port bits with simple statements
    source: [http://www.mikrocontroller.net/attachment/27445/SBIT.C] */
struct bits {
	uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
	uint8_t b7:1;
} __attribute__((__packed__));

#define SBIT(port,pin) ((*(volatile struct bits*)&port).b##pin)

/** Hardware SPI (programmer) Interface	*/
#define AVR_MOSI	( PINB, 3 )		/** hardware SPI MOSI pin	*/
#define AVR_MISO	( PORTB, 4 )	/** hardware SPI MISO pin	*/
#define AVR_SCK		( PINB, 5 )		/** hardware SPI clock pin	*/


/** SPI Port for Soft SPI, outputs	*/
#define SPI_CLK			SBIT( PORTC, 3 )	/** SPI clock line		*/
#define SPI_DAT			SBIT( PORTC, 5 )	/** SPI data line		*/
#define SPI_CS1_ATT		SBIT( PORTC, 4 )	/** SPI for attenuator	*/
#define SPI_CS2_LED		SBIT( PORTB, 6 )	/** SPI for LEDs		*/

/** ports for Multiplexer control, outputs	*/
#define MUX_AUDIO_OFF		SBIT( PORTB, 7 )	/** disables output signal from multiplexer	*/
#define MUX_SOURCE_SEL_0	SBIT( PORTC, 0 )	/** multiplexer select signal bit 0			*/
#define MUX_SOURCE_SEL_1	SBIT( PORTC, 1 )	/** multiplexer select signal bit 1			*/

/** ports for power amplifier control and status	*/
#define AMP_ENABLE			SBIT( PORTC, 2 )	/** enables output stage of power amplifier	*/
#define AMP_WARNING			SBIT( PIND, 4 )		/** input for amplifier warning				*/
#define AMP_ERROR			SBIT( PIND, 5 )		/** input for amplifier error				*/

/** ports for incremental encoder, inputs	*/
#define VOL_A	SBIT( PIND, 1 )	/** incremental encoder output A	*/
#define VOL_B	SBIT( PIND, 2 )	/** incremental encoder output B	*/

/** port for source select switch	*/
#define SOURCE_BUTTON_n	SBIT( PIND, 3 ) /** input for source select button, low active	*/
#define SOURCE_MIN		1				/** minimum allowed value for source			*/
#define SOURCE_MAX		4				/** maximum allowed value for source			*/

/** port for IR data input	*/
#define IR_DATA		SBIT( PIND, 6 ) /** input for data from IR receiver	*/

/** direction defines according to above defines: 0=input, 1=output	*/
#define DDRB_SETTING	0xE8	/** Port B direction setting oooi_oiii	*/
#define DDRC_SETTING	0x3F	/** Port C direction setting iioo_oooo	*/
#define DDRD_SETTING	0x00	/** Port D direction setting iiii_iiii */

/** operation specific defines	*/
#define RESTART		true	/* in case of power up the restart parameter is used to recall saved settings	*/

/** volume specific defines	*/
#define MUTE_VAL	0x7F	/* sending this value to the attenuator mutes the volume setting				*/
#define VOLUME_MIN	0x7E	/* sending this value to the attenuator sets the lowest possible volume			*/
#define VOLUME_MAX	0x00	/* sending this value to the attenuator sets the highest possible volume		*/

/** project functions prototypes	*/
/** get state of current source request	*/
uint8_t get_source(uint8_t startup);

/** get state of current volume request and store a given absolute value	*/
uint8_t get_volume(int8_t value, uint8_t startup_value);

/** save volume and source setting to eeprom	*/
void save_volume(uint8_t volume_r, uint8_t volume_l);
void save_source(uint8_t source);

/** recall volume and source settings from eeprom (after power up)	*/
uint8_t recall_volume (uint8_t right1_or_left0);	/** right = 1; left = 0	*/
uint8_t recall_source (void);	/** since there is only one source byte there is nothing to specify	*/

/** reserves the space in EEPROM for non volatile data	*/
uint8_t ee_volume_left;
uint8_t ee_volume_right;
uint8_t ee_source;

/** output functions	*/

/** this function sets the LEDs via Soft SPI
	vector for Leds: 8 bit, lowes 4 = source(1..4), highest 4 = volume(5..8)
	[v4,v3,v2,v1,s4,s3,s2,s1]

	volume is set with inverse values: 0 = highest volume, 0x7E = lowest volume
	volume must be converted to 4 bits according to volume,
	all volume LEDs will be set at highest volume, at lowest, one LED will be set
	if setting volume LEDs, source parameter must be 0, volume level can be 0	*/
void set_leds(uint8_t source, uint8_t volume);

/** this function sets the volume via Soft SPI and triggers the volume LED output	*/
void set_volume(uint8_t volume_right, uint8_t volume_left);

/** this function sends the actual data via Soft SPI
	since there are only two SPI slaves no additional efford for more universal usage is done
	for LED SPI slave there are only 8 bit of data to be sent
	for volume control two 8 bit values (left and right) need to be sent
	so if the first parameter is 0 it is assumed, that volume data is to be sent
	if the first parameter is not 0 it is asumed that LED data is to be sent*/
void spi_send(uint8_t led, uint8_t volume_r, uint8_t volume_l);

/** this function puts all data bits of the data parameter to SPI_DAT MSB first
 * and generates the SPI clock accordingly	*/
void spi_data_out(uint8_t data);

/** set source for multiplexer	*/
void set_source	(uint8_t source);

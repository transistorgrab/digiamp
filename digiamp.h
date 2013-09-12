/** project file for all project specific defines and functions	*/
/** project uses ATMEGA88AU (http://www.atmel.com/images/doc2545.pdf)	*/

/** LEDs	*/
#define LED_SOURCE_1
#define LED_SOURCE_2
#define LED_SOURCE_3
#define LED_SOURCE_4

#define LED_VOLUME_1
#define LED_VOLUME_2
#define LED_VOLUME_3
#define LED_VOLUME_4

/** defines for ports **
**
** hardware pin configuration:
** all not defined ports are not used or not available	*/

/** function and struct for setting single port bits	
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
#define MUX_AUDIO_OFF		SBIT( PORTC, 7 )	/** disables output signal from multiplexer	*/
#define MUX_SOURCE_SEL_0	SBIT( PORTC, 0 )	/** multiplexer select signal bit 0			*/
#define MUX_SOURCE_SEL_1	SBIT( PORTC, 1 )	/** multiplexer select signal bit 1			*/

/** ports for power amplifier control and status	*/
#define AMP_ENABLE			SBIT( PORTC, 2 )	/** enables output stage of power amplifier	*/
#define AMP_WARNING			SBIT( PIND, 4 )		/** input for amplifier warning				*/
#define AMP_ERROR			SBIT( PIND, 5 )		/** input for amplifier error				*/

/** ports for incremental encoder, inputs	*/
#define VOL_A	SBIT( PIND, 1 )	/** incremental encoder output A	*/
#define VOL_B	SBIT( PIND, 2 )	/** incremental encoder output A	*/

/** port for source select switch	*/
#define SOURCE_BUTTON_n	SBIT( PIND, 3 ) /** input for source select button, low active	*/
#define SOURCE_MIN		1				/** minimum allowed value for source			*/
#define SOURCE_MAX		4				/** maximum allowed value for source			*/

/** port for IR data input	*/
#define IR_DATA		SBIT( PIND, 6 ) /** input for data from IR receiver	*/

/** direction defines according to above defines: 0=input, 1=output	*/
#define DDRB_SETTING	0x68	/** Port B direction setting iooi_oiii	*/
#define DDRC_SETTING	0x3F	/** Port C direction setting iioo_oooo	*/
#define DDRD_SETTING	0x00	/** Port D direction setting	iiii_iiii */

/** project functions prototypes	*/

/** get state of current source request	*/
int get_source(bool restart);

/** get state of current volume request	*/
int get_volume(bool restart);

/** save volume and source setting to eeprom	*/
void save_settings(int volume, int source);

/** recall volume and source settings from eeprom (after power up)	*/
int recall_volume (void);
int recall_source (void);


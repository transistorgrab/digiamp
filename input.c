/** this file defines the input functions for source and volume	*/

#include <avr/io.h>
#include "digiamp.h"

#define MAX_VOLUME_CHANGE 80	/** highest allowed volume change per step, obey int8_t limits!	*/
#define MIN_VOLUME_CHANGE -80	/** lowest allowed volume change per step	*/

/** get state of current source request	
    read button input and return source according to button action
	
	returns: int value of selected source (1..4)
	param: startup can be initialized with a value */
uint8_t get_source(uint8_t startup)
{
	static uint8_t source;	/** variable that holds the current source setting	*/
	static uint8_t old_button_state;	/** variable that holds the old value of button,
										used to switch source only one per click	*/
	if (startup)	/** set source to a previously restored value	*/
		source = startup;
		
	if (!SOURCE_BUTTON_n)	/** button is low active, if high no action is required	*/
	{
		if (old_button_state)	/** button was not pressed	*/
		{
			old_button_state = 0; /** button is pressed	*/
			source++;
			if (source > SOURCE_MAX)
				source = SOURCE_MIN;
		}
	}
	else	/** button is not pressed (any more)	*/
		old_button_state = 1;

	return source;
}

/** get state of current volume setting or set new volume
	parameter value is used for setting volume up (positive value)
			or down	(negative value)
	for requests of current volume value needs to be 0	*/
uint8_t get_volume(int8_t value, uint8_t startup_value)
{
	static uint8_t volume_r;	/** current state: only one volume setting for both channels	*/
	static int8_t volume_updates;	/** counter for volume updates during one schedule cycle	*/
	
	if (startup_value)			/** on startup a from eeprom restored value is given			*/
		volume_r = startup_value;

	if (value)
	{
		/** change volume	*/
		volume_r -= (value+volume_updates);	/** "add" value to current volume	*/
		if ((volume_updates<MIN_VOLUME_CHANGE)||(volume_updates>MAX_VOLUME_CHANGE)) /** below max allowed limits?	*/
			volume_updates += value<<1;	/** dynamic volume change during current schedule cycle	*/

		/** remember: volume is negative proportional to volume value, allow no overflow */
		/** limit volume settings	*/
		if ( ((volume_r <= VOLUME_MAX) && (value > 0)) || ((volume_r > VOLUME_MAX) && (volume_r > VOLUME_MIN) && (value > 0)) )	/** do not allow volume settings < 0 (volume max)	*/
		{
			volume_r = VOLUME_MAX;
			volume_updates = 0;		/* prevent further build-up of acceleration and therefore overflow	*/
		}
		if ((volume_r >= MUTE_VAL) && (value < 0))	/** do not allow volume setting > 127 (mute)	*/
		{
			volume_r = MUTE_VAL;
			volume_updates = 0;
		}
		return volume_r;
	}
	volume_updates = 0;	/** reset counter each schedule cycle	*/	
	return volume_r;	/** send current setting back	*/
}

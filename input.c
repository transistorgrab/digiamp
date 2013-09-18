/** this file defines the input functions for source and volume	*/

#include <avr/io.h>
#include "digiamp.h"

/** get state of current source request	
    read button input and return source according to button action
	
	returns: int value of selected source (1..4) */
uint8_t get_source(void)
{
	static uint8_t source=SOURCE_MIN;	/** variable that holds the current source setting	*/
	static uint8_t old_button_state=1;	/** variable that holds the old value of button,
										used to switch source only one per click	*/

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
uint8_t get_volume(int8_t value)
{
	static uint8_t volume_r; /** current state: only one volume setting for both channels	*/
	
	if(value)	/** request to change volume up or down	*/
	{
		if (!((volume_r <= VOLUME_MAX)||(volume_r >= VOLUME_MIN))) /** remember: volume is negative proportional to volume value, allow no overflow */
			volume_r -= value;	/** "add" value to current volume	*/
	}
	return volume_r;	/** send current setting back	*/
}

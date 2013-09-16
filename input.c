/** this file defines the input functions for source and volume	*/

#include "digiamp.h"
#include <stdbool.h>

/** get state of current source request	
    read button input and return source according to button action
	param: restart	if TRUE then the value from EEPROM is read
	
	returns: int value of selected source (1..4) */
uint8_t get_source(void)
{
	static uint8_t source=SOURCE_MIN;	/** variable that holds the current source setting	*/
	static uint8_t old_button_state=1;	/** variable that holds the old value of button,
										used to switch source only one per click	*/

	if (restart)			/** after reset the old value can be recalled			*/
		source = recall_source();
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

}

/** get state of current volume request	*/
uint8_t get_volume(void)
{
	return;
}

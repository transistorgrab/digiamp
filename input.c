/** this file defines the input functions for source and volume	*/

#include "digiamp.h"
#include <stdbool.h>

/** get state of current source request	
    read button input and return source according to button action
	param: restart	if TRUE then the value from EEPROM is read
	
	returns: int value of selected source (1..4) */
int get_source(bool restart)
{
	static int source=SOURCE_MIN;	/** variable that holds the current source setting	*/
	static bool old_button_state=true;	/** variable that holds the old value of button,
										used to switch source only one per click	*/

	if (restart)			/** after reset the old value can be recalled			*/
		source = recall_source();
	if (!SOURCE_BUTTON_n)	/** button is low active, if high no action is required	*/
	{
		if (old_button_state)	/** button was not pressed	*/
		{
			old_button_state = false; /** button is pressed	*/
			source++;
			if (source > SOURCE_MAX)
				source = SOURCE_MIN;
		}
		else
		{
			old_button_state = true;
		}
	}
}

/** get state of current volume request	*/
int get_volume(void);

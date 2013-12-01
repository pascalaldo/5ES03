#ifndef CONTROL1_H
#define CONTROL1_H

#include <Control.h>

//! Implementation of the Control class for assignment 1
/*!
 * The bot drives around randomly staying within the area marked by white tape.
 * When the bot sees another bot, it stops and transfers its light to the other bot.
 */
class Control1 : public Control
{
	public:
	//! Initialize the class.
	Control1();
	//! Implementation of the Control::doLoop() function. This is executed every 'arduino loop'.
	void doLoop();
	//! Set the right states for every situation of assignment 1.
	void updateState();
	//! Whether the robot currently has the light.
	/*!
	 * @return Whether the robot currently has the light.
	 */
	bool hasLight();
	//! Set whether the robot currently has the light.
	/*!
	 * If the status of the light has changed, the function will either fade the light in or
	 * fade it out.
	 * @param value The new status of the light. True will turn the light on, False will turn it off.
	 */
	void setHasLight(bool value);
	
	private:
	//! Stops the robot, because another robot has been seen.
	/*!
	 * The function also updates the variable that
	 * holds where the robot was seen. @see positionOther
	 * @param pos The position where the other robot was seen.
	 */
	void stopForOther(ControlEnums::Direction pos);
	//! Transfer the light.
	/*!
	 * Sets the current control state to STARTTRANSFERLIGHT, which will initiate the transfer of the light
	 * to the other robot.
	 */
	void transferLight();
	ControlEnums::Direction positionOther; //!< Position where the other robot was seen.
	bool _hasLight; //!< Private variable that holds whether the robot currently has the light.
};

#endif

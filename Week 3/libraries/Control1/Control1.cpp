#include <defs.h>
#include "Arduino.h"
#include "Control1.h"

Control1::Control1()
{
	// Set the initial values for all variables.
	currentState = ControlEnums::STRAIGHTAHEAD;
	positionOther = ControlEnums::STRAIGHT;
	_hasLight = false;
	setHasLight(false);
}

bool Control1::hasLight()
{
	return _hasLight;
}

void Control1::setHasLight(bool value)
{
	// Only do something if the new value is different from the old one.
	if (value != _hasLight)
	{
		// Fade in or out
		for (int i = 0; i < 256; i++)
		{
			setRGBLED(0, (_hasLight ? (255-i) : i), 0);
			// 256*10ms = 2.56 s
			delay(10);
		}
		_hasLight = value;
	}
}

void Control1::doLoop()
{
	keepDistance(); // Look straight ahead.
	lookToSides(); // Look to the sides.
	// TODO: Remove and replace by correct starting sequence
	if (currentState == ControlEnums::WAITTOSTART && motorDist_l < 0.1){
		currentState = ControlEnums::STRAIGHTAHEAD;
	}
	if (distanceFront <= DIST_THRESHOLD){
		stopForOther(ControlEnums::STRAIGHT);
	}else if (distanceRight <= DIST_THRESHOLD){
		stopForOther(ControlEnums::RIGHT);
	}/*else if (distanceLeft <= DIST_THRESHOLD){
		stopForOther(ControlEnums::LEFT);
	}*/
	// TODO NETWORKING: Connect signal for the transfer of the light
	if (distanceLeft <= DIST_THRESHOLD){
		transferLight();
	}
	updateState();
	adjustMotor(motorLine_l*motorDist_l,motorLine_r*motorDist_r);
}

void Control1::stopForOther(ControlEnums::Direction pos)
{
	if (currentState != ControlEnums::WAITTOSTART){
		currentState = ControlEnums::STOPFOROTHER; // Will stop the robot when the state function is executed.
		positionOther = pos;
		// TODO NETWORKING: Send out signal that robot has seen the other (at pos)
	}
}

void Control1::transferLight()
{
	// If the robot is waiting, transfer the light. Otherwise nothing will happen.
	if (currentState == ControlEnums::STOPFOROTHER){
		currentState = ControlEnums::STARTTRANSFERLIGHT;
	}
}

/*!
 * The robot starts in the state:
 * _WAITTOSTART_
 * This state has to be changed manually on some external signal. The next state will be:
 * _STRAIGHTAHEAD_
 * This state will cause the robot to just go straightahead untill it encounters something. The
 * first option is that it will encounter another robot (the function stopForOther(Direction pos)
 * will have to be executed for this) (1). The other option is a white line (2).
 * (1) The state will be changed to _STOPFOROTHER_ (by the function stopForOther(Direction pos)). The robot
 *     do nothing when in this state, untill the function transferLight() is executed. This function will change the state
 *     into _CORRECTLEFT_ or _CORRECTRIGHT_, whichever will cause the bot to turn away from the other bot. After this the 
 *     state will be changed back to _STRAIGHTAHEAD_.
 * (2) The state will be changed to _CORRECTLEFT_ or _CORRECTRIGHT_, dependent on where the line was seen. In these states
 *     the robot will turn around its axis for approximately 50 degrees and then change its state back to _STRAIGHTAHEAD_.
 * 
 * <pre>
 * WAITTOSTART --[signal]--> STRAIGHTAHEAD --[dist  sensors]--> STOPFOROTHER --[transfer light]
 *                                 ^    |                                            v
 *                                 |    |--------[line sensors]--------> CORRECTLEFT or CORRECTRIGHT
 *                                 |                                                 |
 *                                 |-------------------------------------------------|
 * </pre>
 * @see stopForOther(ControlEnums::Direction pos), transferLight()
 */
void Control1::updateState()
{
	if (currentState == ControlEnums::CORRECTLEFT || currentState == ControlEnums::CORRECTRIGHT){
		delay(600); // 600ms will turn the bot ca. 50 degrees.
	}
	
	readSensors();
	
	// Start with 0%
	setMotorLine(0.0f, 0.0f);
	
	ControlEnums::State newstate = ControlEnums::STRAIGHTAHEAD;
	if (currentState == ControlEnums::STRAIGHTAHEAD){
		if (currentPosition == ControlEnums::OFFTRACK){
			newstate = ControlEnums::STRAIGHTAHEAD;
		}else if (currentPosition == ControlEnums::HALFTRACK && ratio > 0){
			newstate = ControlEnums::CORRECTRIGHT;
		}else{
			newstate = ControlEnums::CORRECTLEFT;
		}
	}else if (currentState == ControlEnums::CORRECTLEFT || currentState == ControlEnums::CORRECTRIGHT){
		newstate = ControlEnums::STRAIGHTAHEAD;
	}else if (currentState == ControlEnums::STOPFOROTHER){
		newstate = ControlEnums::STOPFOROTHER;
	}else if (currentState == ControlEnums::STARTTRANSFERLIGHT){
		newstate = ControlEnums::TRANSFERLIGHT;
	}else if (currentState == ControlEnums::TRANSFERLIGHT){
		if (positionOther == ControlEnums::RIGHT){
			newstate = ControlEnums::CORRECTLEFT;
		}else{
			newstate = ControlEnums::CORRECTRIGHT;
		}
	}
	
	if (newstate == ControlEnums::STRAIGHTAHEAD){
		setMotorLine(1.0f, 1.0f);
		#ifdef DEBUG
		setRGBLED(0, 255, 0);
		#endif
	}else if (newstate == ControlEnums::CORRECTLEFT){
		setMotorLine(-0.8f, 0.8f);
		#ifdef DEBUG
		setRGBLED(0, 0, 255);
		#endif DEBUG
	}else if (newstate == ControlEnums::CORRECTRIGHT){
		setMotorLine(0.8f, -0.8f);
		#ifdef DEBUG
		setRGBLED(0, 0, 255);
		#endif
	}else if (newstate == ControlEnums::STOPFOROTHER){
		setMotorLine(0.0f, 0.0f);
		#ifdef DEBUG
		setRGBLED(255, 0, 0);
		#endif
	}else if (newstate == ControlEnums::TRANSFERLIGHT){
		setHasLight(!hasLight());
		setMotorLine(0.0f, 0.0f);
	}
	
	currentState = newstate;
}

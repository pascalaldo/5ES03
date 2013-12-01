#include <defs.h>
#include "Arduino.h"
#include "Control2.h"

Control2::Control2()
{
	// Set the initial values of the variables.
	out_at = ControlEnums::STRAIGHT;
	cross_next_gap = false;
	largecorrectsteps = 300;
	currentState = ControlEnums::STRAIGHTAHEAD;
}

void Control2::doLoop()
{
	keepDistance();
	// TODO NETWORKING: Connect to signal for crossing the gap.
	if (motorDist_l < 0.1){
		startCrossingGap();
	}
	updateState();
	// TODO uncomment this, is commented because distance sensors are now used as replacement for networking
	adjustMotor(motorLine_l/**motorDist_l*/,motorLine_r/**motorDist_r*/);
}

void Control2::startCrossingGap()
{
	// Only cross the gap if the bot is actually waiting for a gap.
	if (isWaitingForGap()){
		currentState = ControlEnums::STARTCROSSGAP;
	}
}

bool Control2::isWaitingForGap()
{
	return (currentState == ControlEnums::WAITFORGAP && !cross_next_gap);
}

/*!
 * The bot starts in the state _STRAIGHTAHEAD_, which makes the robot just go straight ahead.
 * When one of the line sensors sees something black, the bot will adjust its course using the
 * _SMALLCORRECT_ state. When both sensors get off the track, it could be a gap or a sharp bend.
 * To find out the bot will go into the _CONFIRMGAP_ state and will discover a small area in front
 * it. When no track is found, there was a bend, so first go back using the _RETURNTOBENDSTART_ state
 * and then apply a _LARGECORRECT_. When the track was actually found again in the _CONFIRMGAP_ state,
 * there is a gap. The bot will travel back just a little in the _RETURNTOGAPSTART_ state and will then
 * wait for confirmation from the other bot in the _WAITFORGAP_ state. When this conformation is
 * received the startCrossingGap() function should be executed, which sets the state to _STARTCROSSGAP_.
 * This state will initiate the _CROSSGAP_ state, which will let the robot cross the gap and return to
 * either _STRAIGHTAHEAD_, _SMALLCORRECT_ or _LARGECORRECT_.
 * 
 * <pre>
 *        |---------------------------------------------------[external signal]--|
 *        v                                                                      |
 *  STRAIGHTAHEAD --[off track]--> CONFIRMGAP -----> RETURNTOGAPSTART ----> WAITFORGAP 
 *    ^[line]v                         |-----------> RETURNTOBENDSTART 
 *  SMALLCORRECT                                            |
 *        ^                                                 |
 *        |----------------------- LARGECORRECT <-----------|
 * </pre>
 * @see startCrossingGap(), isWaitingForGap()
 */
void Control2::updateState()
{
	int line_left, line_right;
	
	// In these situations, execute the movement for a set amount of time.
	if (currentState == ControlEnums::CONFIRMGAP){
		delay(200);
	}else if (currentState == ControlEnums::CROSSGAP){
		delay(300);
	}else if (currentState == ControlEnums::RETURNTOGAPSTART){
		delay(200);
	}else if(currentState == ControlEnums::RETURNTOBENDSTART){
		delay(250);
	}
	
	// Look for the line.
	readSensors();
	
	// Start with 0%
	setMotorLine(0.0f, 0.0f);
	
	// Control what the next state is going to be.
	ControlEnums::State newstate = ControlEnums::STRAIGHTAHEAD;
	if (currentState == ControlEnums::STRAIGHTAHEAD || currentState == ControlEnums::SMALLCORRECT || currentState == ControlEnums::CROSSGAP){
		if (currentPosition == ControlEnums::ONTRACK){
			newstate = ControlEnums::STRAIGHTAHEAD;
		}else if (currentPosition == ControlEnums::HALFTRACK){
			newstate = ControlEnums::SMALLCORRECT;
		}else{
			newstate = ControlEnums::CONFIRMGAP;
		}
	}else if (currentState == ControlEnums::CONFIRMGAP){
		//Serial.println("ConfirmGap");
		if (currentPosition != ControlEnums::OFFTRACK){
			if (cross_next_gap){
				if (currentPosition == ControlEnums::HALFTRACK){
					newstate = ControlEnums::SMALLCORRECT;
				}else{
					newstate = ControlEnums::STRAIGHTAHEAD;
				}
				cross_next_gap = false;
			}else{
				newstate = ControlEnums::RETURNTOGAPSTART;
			}
		}else{
			newstate = ControlEnums::RETURNTOBENDSTART;
		}
	}else if (currentState == ControlEnums::RETURNTOGAPSTART){
		newstate = ControlEnums::WAITFORGAP;
		// TODO Networking: Send signal that robot is waiting
	}else if (currentState == ControlEnums::RETURNTOBENDSTART){
		newstate = ControlEnums::LARGECORRECT;
		largecorrectsteps = 300;
	}else if (currentState == ControlEnums::WAITFORGAP){
		newstate = ControlEnums::WAITFORGAP;
	}else if (currentState == ControlEnums::LARGECORRECT){
		if (currentPosition == ControlEnums::ONTRACK){
			newstate = ControlEnums::STRAIGHTAHEAD;
		}else if (currentPosition == ControlEnums::HALFTRACK){
			newstate = ControlEnums::SMALLCORRECT;
		}else{
			newstate = ControlEnums::LARGECORRECT;
			largecorrectsteps--;
		}
	}else if (currentState = ControlEnums::STARTCROSSGAP){
		cross_next_gap = true;
		newstate = ControlEnums::CROSSGAP;
	}
	
	// Set the motor speed accordingly.
	if (newstate == ControlEnums::STRAIGHTAHEAD){
		setMotorLine(1.0f, 1.0f);
		out_at = ControlEnums::STRAIGHT;
		setRGBLED(0, 255, 0);
	}else if (newstate == ControlEnums::SMALLCORRECT){
		if (ratio < 0){
			// The bot is seeing the line at the right side and is therefore heading too far to the left.
			// So the bot should turn right.
			//Serial.println("GOTO RIGHT");
			out_at = ControlEnums::LEFT; // Keep track of where the line was last seen.
			// Decrease the speed of the right motor to go right.
			setMotorLine(1.0f, max(0, (1.0f+LINE_FACTOR*ratio)), TURN_SPEED);
		}else{
			// The bot is seeing the line at the left side and is therefore heading too far to the right.
			// So the bot should turn left.
			//Serial.println("GOTO LEFT");
			out_at = ControlEnums::RIGHT; // Keep track of where the line was last seen.
			// Decrease the speed of the left motor to go left.
			setMotorLine(max(0, (1.0f-LINE_FACTOR*ratio)), 1.0f, TURN_SPEED);
		}
		setRGBLED(0, 0, 255);
	}else if (newstate == ControlEnums::CONFIRMGAP){
		setMotorLine(1.0f, 1.0f);
		setRGBLED(0, 120, 120);
	}else if (newstate == ControlEnums::RETURNTOGAPSTART || newstate == ControlEnums::RETURNTOBENDSTART){
		setMotorLine(-1.0f, -1.0f);
		//Serial.println("Go Back!");
		setRGBLED(0, 255, 100);
	}else if (newstate == ControlEnums::WAITFORGAP){
		setMotorLine(0.0f, 0.0f);
		setRGBLED(255, 0, 0);
	}else if (newstate == ControlEnums::LARGECORRECT){
		float oal = (out_at == ControlEnums::LEFT ? -1.0f : 1.0f);
		if (largecorrectsteps > 200){
			setMotorLine(-0.8f, 0.8f, oal);
		}else if (largecorrectsteps > 0){
			setMotorLine(0.8f, -0.8f, oal);
		}else if (largecorrectsteps > -100){
			setMotorLine(-0.8f, 0.8f, oal);
		}else if (largecorrectsteps > -150){
			setMotorLine(0.8f, 0.8f);
		}else{
			setMotorLine(0.8f, 0.8f);
			largecorrectsteps = 300;
		}
		setRGBLED(255, 0, 100);
	}else if (newstate == ControlEnums::CROSSGAP){
		setMotorLine(1.0f, 1.0f);
		setRGBLED(0, 255, 50);
	}
	
	currentState = newstate;
}

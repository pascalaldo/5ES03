#ifndef CONTROL2_H
#define CONTROL2_H

#include <Control.h>

//! Implementation of the Control class for assignment 2
/*!
 * The bot drives over the white line untill it encounters a black interruption. The bot will check
 * whether this is a gap indicating a intersection. Whent it is indeed an intersection the bot wil wait
 * for communication with the other bot. If it is not a gap, the black interruption is a bend and the bot
 * will go back and try to follow the bending track.
 */
class Control2 : public Control
{
	public:
	//! Initialize the class.
	Control2();
	//! Implementation of the Control::doLoop() function. This is executed every 'arduino loop'.
	void doLoop();
	//! Initiate the crossing of the gap, should be executed when communication was successfull.
	void startCrossingGap();
	//! Returns whether the bot is currently waiting for a gap.
	bool isWaitingForGap();
	//! Set the right states for every situation of assignment 2.
	void updateState();
	
	private:
	//! Keep track of where the bot leaves the track.
	ControlEnums::Direction out_at;
	//! Whether to cross the next gap. Is set to true after crossing a gap before a intersection.
	bool cross_next_gap;
	//! Keep track of how far to turn when looking for the track.
	int largecorrectsteps;
};

#endif

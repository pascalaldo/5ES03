#ifndef CONTROL_H
#define CONTROL_H

#include <defs.h>

struct ControlEnums
{
public:
	enum State{
		// Both
		STRAIGHTAHEAD,
		// Second assignment
		SMALLCORRECT,
		LARGECORRECT,
		CONFIRMGAP,
		RETURNTOGAPSTART,
		RETURNTOBENDSTART,
		WAITFORGAP,
		STARTCROSSGAP,
		CROSSGAP,
		// First assignment
		WAITTOSTART,
		CORRECTLEFT,
		CORRECTRIGHT,
		STOPFOROTHER,
		STARTTRANSFERLIGHT,
		TRANSFERLIGHT
	};

	enum Direction{
		LEFT,
		RIGHT,
		STRAIGHT
	};
	enum Position{
		OFFTRACK,
		ONTRACK,
		HALFTRACK
	};
};

  class Control
 {
   public:
   Control();
   void updateState();
   void doLoop();
   void setRGBLED(int r, int g, int b);
   void keepDistance();
   void lookToSides();
   void adjustMotor(float left, float right);
   
   protected:
   ControlEnums::State currentState;
   
   float currentSpeed_l; //current left engine speed
   float currentSpeed_r; //current right engine speed
   
   float motorLine_l; // Fraction of speed for the left motor with line [-1,1]
   float motorLine_r; // Fraction of speed for the right motor with line [-1,1]
   
   float motorDist_l; // Fraction of speed for the left motor with dist [-1,1]
   float motorDist_r; // Fraction of speed for the right motor with dist [-1,1]
   
   int distanceFront;
   int distanceLeft;
   int distanceRight;
   
   void readSensors();
   ControlEnums::Position currentPosition;
   float ratio;
 };
 
#endif

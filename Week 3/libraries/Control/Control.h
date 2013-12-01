#ifndef CONTROL_H
#define CONTROL_H

#include <defs.h>

//! Struct that holds various enums for the control class.
struct ControlEnums
{
	public:
	//! The enum State holds the different control states of the bots.
	/*!
	 * Some states apply to assignment 1, others to assignment 2 or both.
	 */
	enum State{
		// For both assignments
		STRAIGHTAHEAD, //!< Go straight ahead
		// Second assignment
		SMALLCORRECT,		//!< Apply a small correction to stay on track
		LARGECORRECT,		//!< Apply a large correction to get back on track
		CONFIRMGAP,			//!< Move forward a little to check whether there is a gap
		RETURNTOGAPSTART,	//!< Go back to the start of the gap
		RETURNTOBENDSTART,	//!< Go back to the start of the bend
		WAITFORGAP,			//!< Wait at the intersection for the other robot
		STARTCROSSGAP,		//!< Start crossing the gap (to be used to initiate the crossing)
		CROSSGAP,			//!< Actually cross the gap
		// First assignment
		WAITTOSTART,		//!< Wait for the right information before starting
		CORRECTLEFT,		//!< Stay within the borders by a correction to the left
		CORRECTRIGHT,		//!< Stay within the borders by a correction to the right
		STOPFOROTHER,		//!< Stop because the other robot was seen
		STARTTRANSFERLIGHT,	//!< Start the transfer of the light (to be used to initiate the transfer)
		TRANSFERLIGHT		//!< Actually transfer the light
	};
	
	//! The enum Direction defines a direction or possibly a relative location.
	enum Direction{
		LEFT,
		RIGHT,
		STRAIGHT
	};
	
	//! The enum Position defines a position of the bot with respect to the track.
	enum Position{
		OFFTRACK,			//!< Both sensors sense that the bot is on black ground
		ONTRACK,			//!< Both sensors sense that the bot is on white ground
		HALFTRACK			//!< One of the sensors senses that the bot is on black ground
	};
};

//! The base class for movement control of the AdMoVeo.
/*!
 * Includes functions to read different sensors and adjust the motor speed.
 * Basics for State based bot control are also implemented here.
 */
class Control
{
	public:
	//! Initiate the class.
	Control();
	//! Calculate the next state of the robot and act accordingly.
	void updateState();
	//! Execute one loop step. NOTE: Should be overriden when inheriting this class.
	void doLoop();
	//! Set the RGB LED. NOTE: Should be overriden when inheriting this class.
	/*!
	 * @param r The value of the red channel [0...255].
	 * @param g The value of the green channel [0...255].
	 * @param b The value of the blue channel [0...255].
	 */
	void setRGBLED(int r, int g, int b);
	//! Read the front distance sensor and adjust the speed.
	void keepDistance();
	//! Read the left and right distance sensors and store the values.
	/*!
	 * @see distanceLeft, distanceRight
	 */
	void lookToSides();
	//! Adjust the motor speed to the provided values.
	/*!
	 * Here the speed for each motor is defined as a float between 1 and -1
	 * to go full speed straight ahead or back respectively.
	 * @param left Speed of the left motor [-1...1].
	 * @param right Speed of the right motor [-1...1].
	 */
	void adjustMotor(float left, float right);
	
	protected:
	ControlEnums::State currentState;
	
	float currentSpeed_l; //!< Current left engine speed.
	float currentSpeed_r; //!< Current right engine speed.
	
	float motorLine_l; //!< Speed for the left motor to correct for the deviation from the line.
	float motorLine_r; //!< Speed for the right motor to correct for the deviation from the line.
	//! Set the speed of the motors to correct for the deviation from the line.
	/*!
	 * This function does not yet adjust the motor speed, it only stores the speed to be used later
	 * to set the motor to.
	 * @param left Speed of the left motor [-1..1].
	 * @param right Speed of the right motor [-1..1].
	 * @see setMotorLine(float left, float right, float factor)
	 */
	void setMotorLine(float left, float right);
	//! Set the speed of the motors to correct for the deviation from the line using a certain factor.
	/*!
	 * This function does not yet adjust the motor speed, it only stores the speed to be used later
	 * to set the motor to.
	 * @param left Speed of the left motor [-1..1].
	 * @param right Speed of the right motor [-1..1].
	 * @param factor Factor to multiply both the left and right speeds with.
	 * @see setMotorLine(float left, float right)
	 */
	void setMotorLine(float left, float right, float factor);
	
	float motorDist_l; //!< Speed for the left motor to correct for the distance to a object [-1...1].
	float motorDist_r; //!< Speed for the right motor to correct for the distance to a object  [-1..1].
	//! Set the speed of the motors to correct for the distance to a object.
	/*!
	 * This function does not yet adjust the motor speed, it only stores the speed to be used later
	 * to set the motor to.
	 * @param left Speed of the left motor [-1..1].
	 * @param right Speed of the right motor [-1..1].
	 * @see setMotorLine(float left, float right)
	 */
	void setMotorDist(float left, float right);
	
	int distanceFront; //!< Distance provided by the front sensor.
	int distanceLeft; //!< Distance provided by the left sensor.
	int distanceRight; //!< Distance provided by the right sensor.
	
	//! Read out the line following sensors.
	void readSensors();
	//! Current position with respect to the track.
	ControlEnums::Position currentPosition;
	//! Ratio between the signal of the right and left line following sensors.
	float ratio;
};

#endif // CONTROL_H

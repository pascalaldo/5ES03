#include <defs.h>
#include "Arduino.h"
#include <Control.h>

Control::Control()
{
	// Define the starting values of all variables.
	currentSpeed_l = 0.0f;
	currentSpeed_r = 0.0f;
	
	motorLine_l = 1.0f;
	motorLine_r = 1.0f;
	
	motorDist_l = 1.0f;
	motorDist_r = 1.0f;
	
	distanceFront = 0;
	distanceLeft = 0;
	distanceRight = 0;
	
	currentState = ControlEnums::STRAIGHTAHEAD;
}

void Control::setRGBLED(int r, int g, int b){
	analogWrite(ID_LED_RED, r);
	analogWrite(ID_LED_GREEN, g);
	analogWrite(ID_LED_BLUE, b);
}

void Control::setMotorLine(float left, float right)
{
	// Make sure the values do not get lower that -1
	motorLine_l = max(-1, left);
	motorLine_r = max(-1, right);
}

void Control::setMotorLine(float left, float right, float factor)
{
	// Make sure the values do not get lower that -1
	motorLine_l = max(-1, factor*left);
	motorLine_r = max(-1, factor*right);
}

void Control::setMotorDist(float left, float right)
{
	// Make sure the values do not get lower that 0
	motorLine_l = max(0, left);
	motorLine_r = max(0, right);
}

void Control::keepDistance(){
	/* Select ADC_DISTANCE FRONT */
	digitalWrite(ID_FRONTREAR, HIGH);
	digitalWrite(ID_LEFTRIGHT, LOW);
	delay(1);
	distanceFront = analogRead(ID_ADC_DISTANCE);
	
	// Stop is the distance is really small
	if(distanceFront <= DIST_THRESHOLD_CRITICAL){
		motorDist_l = 0;
		motorDist_r = 0;
	}else if(distanceFront <= DIST_THRESHOLD){
		// We see something
		motorDist_l *= 0.95; // Slow down when we see something
		motorDist_r *= 0.95;
	}
	else //not seeing anything
	{
		motorDist_l = 1;
		motorDist_r = 1;
	}
}

void Control::lookToSides(){
	// Look to the left.
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, LOW);
	delay(1);
	distanceLeft = analogRead(ID_ADC_DISTANCE);
	
	// Look to the right.
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, HIGH);
	delay(1);
	distanceRight = analogRead(ID_ADC_DISTANCE);
}

void Control::adjustMotor(float motor_l, float motor_r){
	// Stop all motors briefly is the direction of one of the motors has to be changed.
	if (((motor_l >= 0) != (currentSpeed_l >= 0)) ||
	((motor_r >= 0) != (currentSpeed_r >= 0))){
		analogWrite(ID_SPEED_L, 0);
		analogWrite(ID_SPEED_R, 0);
		delay(MOTOR_CORRECTION_DELAY);
	}
	
	// Do the right adjustments to the speed and direction of the motors.
	if (motor_l >= 0){
		if (currentSpeed_l < 0){
			digitalWrite(ID_DIRECTION_L, HIGH);
		}
		analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*motor_l)));
	}else{
		if (currentSpeed_l >= 0){
			digitalWrite(ID_DIRECTION_L, LOW);
		}
		analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*-motor_l)));
	}
	if (motor_r >= 0){
		if (currentSpeed_r < 0){
			digitalWrite(ID_DIRECTION_R, HIGH);
		}
		analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*motor_r)));
	}else{
		if (currentSpeed_r >= 0){
			digitalWrite(ID_DIRECTION_R, LOW);
		}
		analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*-motor_r)));
	}
	
	// Update the currentSpeed values
	currentSpeed_l = motor_l;
	currentSpeed_r = motor_r;
}

void Control::readSensors()
{
	int line_left, line_right;
	// Read out the left line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, LOW);
	delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_left = analogRead(ID_ADC_LINE);
	
	// Read out the right line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, HIGH);
	delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_right = analogRead(ID_ADC_LINE);
	
	// Correct for bot dependent anomalties
	float lineCorrected_l = CORRECTION_LEFT*(float)line_left;
	float lineCorrected_r = CORRECTION_RIGHT*(float)line_right;
	
	// Calculate the ratio between the two sensors and normalize.
	ratio = ((float)(lineCorrected_l-lineCorrected_r)/1000.0f);
	
	// Determine the position with respect to the track.
	currentPosition = ControlEnums::ONTRACK;
	if (lineCorrected_l < LINE_THRESHOLD && lineCorrected_r < LINE_THRESHOLD){
		currentPosition = ControlEnums::OFFTRACK;
	}else if (lineCorrected_l < LINE_THRESHOLD || lineCorrected_r < LINE_THRESHOLD){
		currentPosition = ControlEnums::HALFTRACK;
	}
}

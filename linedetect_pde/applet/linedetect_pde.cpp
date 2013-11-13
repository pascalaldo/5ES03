/* Marvin the Paranoid Arduino */

// Define some IDs
#define ID_ADC_LINE       2
// Same ID?
#define ID_ENCODER_R      2
#define ID_ENCODER_L      4
#define ID_SPEED_R        5
#define ID_SPEED_L        6

#define ID_DIRECTION_R    7
#define ID_DIRECTION_L    8
#define ID_LEFTRIGHT     12
#define ID_FRONTREAR     13

#define ID_LED_BLUE       9
// Try not to use the red LED, since it might interfere 
// with the line detection
#define ID_LED_RED       10
#define ID_LED_GREEN     11

#define DELAY_VAL	  0
#define DELAY_READ_LINE   1

// Should be made dependent on the bot nr.
#define CORRECTION_LEFT   1.0f
#define CORRECTION_RIGHT  1.0f

// Threshold of what is considered line (white)
#define LINE_THRESHOLD  300
// How severe the reaction to seeing the edge of the line is
#define LINE_FACTOR       5.0f
// How long the bot should 'recover' from being off track. This is a delay that
// makes sure the bot does not to quickly assume it is back on track.
#define RECOVERY_STEPS  1000

#define MOTOR_SPEED     255.0f
// Speed fraction to turn around with
#define TURN_SPEED        0.8f

#include "WProgram.h"
void setup();
void detectLine();
void moveDroid();
void loop();
float motor_l; // Fraction of speed for the left motor [-1,1]
float motor_r; // Fraction of speed for the right motor [-1,1]
// Whether the bot went off the track at the left side,
// false when the bot went off the track at the right side
boolean out_at_left; 
int off_track;
// How long the bot should still recover from being off track.
// It is set to >0 when off track and decreases when the line is found again.
// This makes sure the bot does not immediately stop rotating when the line is
// just barely visible.

void setup()
{
  Serial.begin(57600); // Baud rate
  Serial.println("## Marvin the Paranoid Arduino ##");
  
  pinMode(ID_LED_BLUE, OUTPUT);
  analogWrite(ID_LED_BLUE, 0);
  
  pinMode(ID_LEFTRIGHT, OUTPUT);
  digitalWrite(ID_LEFTRIGHT, HIGH);
  
  pinMode(ID_FRONTREAR, OUTPUT);
  digitalWrite(ID_FRONTREAR, HIGH);
  
  /*
   * These two should not be necessary, but are.
   * If they are left out the light sensor readings
   * are incorrect.
   */
  pinMode(ID_DIRECTION_R, OUTPUT);
  digitalWrite(ID_DIRECTION_R, HIGH);
  
  pinMode(ID_DIRECTION_L, OUTPUT);
  digitalWrite(ID_DIRECTION_L, HIGH);
  
  analogWrite(ID_SPEED_L, 0);
  analogWrite(ID_SPEED_R, 0);
  
  // Initialize some variables
  motor_l = 1.0f;
  motor_r = 1.0f;
  out_at_left = false;
  off_track = 0;
}

void detectLine()
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
        float line_l_cor = CORRECTION_LEFT*(float)line_left;
        float line_r_cor = CORRECTION_RIGHT*(float)line_right;

        //Serial.print("Left: "); Serial.println(line_l_cor);
        //Serial.print("Right: "); Serial.println(line_r_cor);

        float ratio = ((float)(line_l_cor-line_r_cor)/1000.0f);
        
        //Serial.print("Ratio: "); Serial.println(ratio);
        
        // Start with 100% power for both motors
        motor_l = 1.0f;
        motor_r = 1.0f;
        
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          // If both values are below the threshold, the bot is off track.
          // Try desperately rotating to get back on the track.
          Serial.println("OFF TRACK");
          // Light up blue LED
          analogWrite(ID_LED_BLUE, 255);
          // Dim the green LED
          analogWrite(ID_LED_GREEN, 0);
          // We're off track!
          off_track = RECOVERY_STEPS;
          if (out_at_left){
            // Try to rotate to the right
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motor_l = 0.9*TURN_SPEED; 
            motor_r = -TURN_SPEED;
          }else{
            // Try to rotate to the left
            motor_l = -TURN_SPEED;
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motor_r = 0.9*TURN_SPEED;
          }
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          // The line is still visible at one side.
          if (off_track > 0){
            // The robot was recently off track, and should keep turning until it is better on the track.
            if (out_at_left){
              // Try to rotate to the right
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motor_l = 0.9*TURN_SPEED; 
              motor_r = -TURN_SPEED;
            }else{
              // Try to rotate to the left
              motor_l = -TURN_SPEED;
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motor_r = 0.9*TURN_SPEED;
            }
            off_track--;
          }else{
            // The bot is somewhat off the track, adjust the direction a little.
            // Dim all LEDS
            analogWrite(ID_LED_BLUE, 0);
            analogWrite(ID_LED_GREEN, 0);
            
            // Which direction should we go to?
            if (ratio < 0){
              // The bot is seeing the line at the right side and is therefore heading too far to the left.
              // So the bot should turn right.
              Serial.println("GOTO RIGHT");
              out_at_left = true; // Keep track of where the line was last seen.
              // Decrease the speed of the right motor to go right.
              motor_r = motor_r+LINE_FACTOR*ratio;
            }else{
              // The bot is seeing the line at the left side and is therefore heading too far to the right.
              // So the bot should turn left.
              Serial.println("GOTO LEFT");
              out_at_left = false; // Keep track of where the line was last seen.
              // Decrease the speed of the left motor to go left.
              motor_l = motor_l-LINE_FACTOR*ratio;
            }
            // Decrease the speed a little so the bot doesn't miss the corner.
            motor_l = motor_l*TURN_SPEED;
            motor_r = motor_r*TURN_SPEED;
          }
        }else{
          // The bot is heading in the correct direction! No adjustments have to be made.
          off_track = 0;
          // Just light up the green light to show everything is okay :)
          analogWrite(ID_LED_BLUE, 0);
          analogWrite(ID_LED_GREEN, 255);
        }
}

// This function should be replaced by the correct motor function.
void moveDroid(){
        if (motor_l > 0){
          digitalWrite(ID_DIRECTION_L, HIGH);
          analogWrite(ID_SPEED_L, ((int)(MOTOR_SPEED*motor_l)));
        }else{
          digitalWrite(ID_DIRECTION_L, LOW);
          analogWrite(ID_SPEED_L, ((int)(MOTOR_SPEED*-motor_l)));
        }
        if (motor_r > 0){
          digitalWrite(ID_DIRECTION_R, HIGH);
          analogWrite(ID_SPEED_R, ((int)(MOTOR_SPEED*motor_r)));
        }else{
          digitalWrite(ID_DIRECTION_R, LOW);
          analogWrite(ID_SPEED_R, ((int)(MOTOR_SPEED*-motor_r)));
        }
}

void loop()
{
        detectLine();
        moveDroid();

	delay(DELAY_VAL);
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}


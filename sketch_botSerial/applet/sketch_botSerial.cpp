/* Serial CD8775
 * This file incorporates all the bits of code into one functional program.
 * Currently implemented:
 * - Hearing the clap (still needs treshold values)
 * - Avoiding collision
 *
 * To implement:
 * - Following the line
 * - Setting speed
 */

#define	ID_ADC_SOUND     0
#define ID_ADC_LINE      2
#define ID_SPEED_R       5
#define ID_SPEED_L       6
#define ID_DIRECTION_R   7
#define ID_DIRECTION_L   8
#define	ID_LEFTRIGHT	12
#define	ID_FRONTREAR	13
#define	ID_ADC_DISTANCE  3

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
void loop();
void adjustMotor(float lSpd, float rSpd);
void hearClap();
void keepDistance();
void detectLine();
float motorLine_l; // Fraction of speed for the left motor with line [-1,1]
float motorLine_r; // Fraction of speed for the right motor with line [-1,1]
float motorDist_l; // Fraction of speed for the left motor with dist [-1,1]
float motorDist_r; // Fraction of speed for the right motor with dist [-1,1]


// Whether the bot went off the track at the left side,
// false when the bot went off the track at the right side
boolean out_at_left; 
int off_track;
// How long the bot should still recover from being off track.
// It is set to >0 when off track and decreases when the line is found again.
// This makes sure the bot does not immediately stop rotating when the line is
// just barely visible.


#include <math.h> //include math functions

#define hasSerial     false

//Voor bot 2
int sound_left, sound_right;
int clap_upper = 400;
int clap_lower = 150;
boolean move = false;
int dist_front;
int dist = 950; //distance at which keepdistance should trigger.


//Voor bot CD8775
/*
int sound_left, sound_right;
int clap_upper = 400;
int clap_lower = 150;
boolean move = false;
int dist_front;
int dist = 50;
*/

static float lCurrentSpd = 0; //current left engine speed
static float rCurrentSpd = 0; //current right engine speed


void setup(){
  move = false;
  
  Serial.begin(57600); //For debugging purposes. Ignore if not needed.
  
  pinMode(ID_LEFTRIGHT, OUTPUT);
  digitalWrite(ID_LEFTRIGHT, HIGH);

  pinMode(ID_FRONTREAR, OUTPUT);
  digitalWrite(ID_FRONTREAR, HIGH);
  
  /*
   * These two should not be necessary, but are.
   * If they are left out the sensor readings
   * are incorrect.
   * Copied without shame.
   */
  pinMode(ID_DIRECTION_R, OUTPUT);
  digitalWrite(ID_DIRECTION_R, HIGH);

  pinMode(ID_DIRECTION_L, OUTPUT);
  digitalWrite(ID_DIRECTION_L, HIGH);
    

  motorLine_l = 1.0f; //value for the left motor for the line
  motorLine_r = 1.0f; //value for the right motor for the line
  motorDist_l = 1.0f; //value for the left motor for the distance
  motorDist_r = 1.0f; //value for the right motor for the distance

  
  out_at_left = false;
  off_track = 0;
}

void loop(){

 
  if(move){
    detectLine();
   // keepDistance();
    adjustMotor(min(motorLine_l,motorDist_l),min(motorLine_r,motorDist_r));
  }
  else
  {
      hearClap();
  }
}

#define ID_ENCODER_R      2
#define ID_ENCODER_L      4
#define ID_SPEED_R        5
#define ID_SPEED_L        6
#define ID_DIRECTION_R    7
#define ID_DIRECTION_L    8

#define DELAY_STOP      500
#define DELAY_DRIVE     1000


/*
@pre: -0.6 <= lSpd <=0.6 due to engine not engaging otherwise
@pre: -0.6 <= rSpd <=0.6 due to engine not engaging otherwise
@param int speed
@param float LRProportion: Proportion that the left motor is stronger than the right motor.
@post: -1 >= lCurrentSpd >= 1
@post: -1 >= rCurrentSpd >= 1
*/
void adjustMotor(float lSpd, float rSpd)
{
  if (lCurrentSpd <=0 && lSpd >0) //left going backward and want forward
  { 
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_L, HIGH); 
  }
  
  if (lCurrentSpd >=0 && lSpd <0) //left going forward and want backward
  { 
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_L, LOW);
  }

  if (rCurrentSpd <=0 && rSpd >0) //right going backward and want forward
  { 
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_R, HIGH);
  }
  if (rCurrentSpd >=0 && rSpd <0) //right going forward and want backward
  { 
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_R, LOW);
  }

  lCurrentSpd = lSpd;
  rCurrentSpd = rSpd;

  lSpd = abs((int) (lSpd*(hasSerial ? 255 : 200))); //get correction
  rSpd = abs((int) (rSpd*(hasSerial ? 251 : 255))); //get correction

  int s1 = (int) lSpd;
  analogWrite(ID_SPEED_L,s1);

  int s2 = (int) rSpd;
  analogWrite(ID_SPEED_R,s2); 
}


void hearClap(){
  /* Select ADC_SOUND LEFT */
  digitalWrite(ID_FRONTREAR, LOW);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(50);
  sound_left = analogRead(ID_ADC_SOUND);

  if(sound_left <= clap_lower || sound_left >= clap_upper){
    move = true;
  }
}



void keepDistance(){
  /* Select ADC_DISTANCE FRONT */
  digitalWrite(ID_FRONTREAR, HIGH);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(1);
  dist_front = analogRead(ID_ADC_DISTANCE);
  if(dist_front <= dist){ //we see something
    motorDist_l *= 0.95; //slow down when we see something
    motorDist_r *= 0.95;
              analogWrite(ID_LED_GREEN, 125);
                        analogWrite(ID_LED_BLUE, 125);
  }
  else //not seeing anything
  {
    motorDist_l = 1;
    motorDist_r = 1;
  }
  
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
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
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
            motorLine_l = 0.9*TURN_SPEED; 
            motorLine_r = -TURN_SPEED;
          }else{
            // Try to rotate to the left
            motorLine_l = -TURN_SPEED;
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motorLine_r = 0.9*TURN_SPEED;
          }
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          // The line is still visible at one side.
          if (off_track > 0){
            // The robot was recently off track, and should keep turning until it is better on the track.
            if (out_at_left){
              // Try to rotate to the right
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_l = 0.9*TURN_SPEED; 
              motorLine_r = -TURN_SPEED;
            }else{
              // Try to rotate to the left
              motorLine_l = -TURN_SPEED;
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_r = 0.9*TURN_SPEED;
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
              motorLine_l = motorLine_l-LINE_FACTOR*ratio;
            }else{
              // The bot is seeing the line at the left side and is therefore heading too far to the right.
              // So the bot should turn left.
              Serial.println("GOTO LEFT");
              out_at_left = false; // Keep track of where the line was last seen.
              // Decrease the speed of the left motor to go left.
              motorLine_r = motorLine_r+LINE_FACTOR*ratio;
            }
            // Decrease the speed a little so the bot doesn't miss the corner.
            motorLine_l = motorLine_l*TURN_SPEED;
            motorLine_r = motorLine_r*TURN_SPEED;
          }
        }else{
          // The bot is heading in the correct direction! No adjustments have to be made.
          off_track = 0;
          // Just light up the green light to show everything is okay :)
          analogWrite(ID_LED_BLUE, 0);
          analogWrite(ID_LED_GREEN, 255);
        }
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}


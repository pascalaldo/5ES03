/* Bot 2
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
int clap_upper = 700;
int clap_lower = 20;
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
    keepDistance();
    adjustMotor(min(motorLine_l,motorDist_l),min(motorLine_r,motorDist_r));
  }
  else
  {
      hearClap();
  }
}


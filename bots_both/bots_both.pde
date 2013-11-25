#define NOT_BOT_SERIAL 1
#define ASSIGNMENT1 1
/* 
 * 
 */
#include <math.h> //include math functions

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

// Motor corrections
#ifdef BOT_SERIAL
#define MOTOR_CORRECTION_L 255
#define MOTOR_CORRECTION_R 251
#else
#define MOTOR_CORRECTION_L 250
#define MOTOR_CORRECTION_R 255
#endif

// Should be made dependent on the bot nr.
#define CORRECTION_LEFT   1.0f
#define CORRECTION_RIGHT  1.0f

// Threshold of what is considered line (white)
#define LINE_THRESHOLD  300
// How severe the reaction to seeing the edge of the line is
#define LINE_FACTOR       2.0f
// How long the bot should 'recover' from being off track. This is a delay that
// makes sure the bot does not to quickly assume it is back on track.
#define RECOVERY_STEPS   50

#define MOTOR_SPEED     255.0f
// Speed fraction to turn around with
#define TURN_SPEED        1.0f

// Distance at which keepdistance should trigger.
#ifdef BOT_SERIAL
#define DIST_THRESHOLD  950
#define DIST_THRESHOLD_CRITICAL  200
#else
#define DIST_THRESHOLD   50
#define DIST_THRESHOLD_CRITICAL  10
#endif

// Clap thresholds
#ifdef BOT_SERIAL
#define CLAP_UPPER      400
#define CLAP_LOWER      100
#else
#define CLAP_UPPER      400
#define CLAP_LOWER      150
#endif

float motorLine_l; // Fraction of speed for the left motor with line [-1,1]
float motorLine_r; // Fraction of speed for the right motor with line [-1,1]
float motorDist_l; // Fraction of speed for the left motor with dist [-1,1]
float motorDist_r; // Fraction of speed for the right motor with dist [-1,1]


// Whether the bot went off the track at the left side,
// false when the bot went off the track at the right side
enum Direction{
  LEFT,
  RIGHT,
  STRAIGHT
};
#ifdef ASSIGNMENT2
enum State{
  STRAIGHTAHEAD,
  SMALLCORRECT,
  LARGECORRECT,
  CONFIRMGAP,
  RETURNTOGAPSTART,
  RETURNTOBENDSTART,
  WAITFORGAP,
  STARTCROSSGAP,
  CROSSGAP
};
#else
enum State{
  WAITTOSTART,
  STRAIGHTAHEAD,
  CORRECTLEFT,
  CORRECTRIGHT,
  STOPFOROTHER
};
#endif

enum Position{
  OFFTRACK,
  ONTRACK,
  HALFTRACK
};

Direction out_at;
boolean cross_next_gap = false;
State curstate;
int largecorrectsteps = 300;

int off_track;
// Count how long the bot has been off track;
int off_track_count = 0;
// How long the bot should still recover from being off track.
// It is set to >0 when off track and decreases when the line is found again.
// This makes sure the bot does not immediately stop rotating when the line is
// just barely visible.

int sound_left, sound_right;
boolean move = false;
int dist_front;

static float lCurrentSpd = 0; //current left engine speed
static float rCurrentSpd = 0; //current right engine speed

// Distance measurement
float measuredDistance = 0.0f;
unsigned long lastSpeedChange = 0;

static int dist_freq_count = 0;

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

  
  out_at = STRAIGHT;
  off_track = 0;
  curstate = STRAIGHTAHEAD;
}

void loop(){
  /*if(move){*/
    /*if(off_track_count > 700){
      analogWrite(ID_LED_BLUE, 0);
      analogWrite(ID_LED_GREEN, 0);
      analogWrite(ID_LED_RED, 255);
      adjustMotor(0,0);
      Serial.print("TOTAL DISTANCE: "); Serial.println(getTotalDistance());
      for(;;);
    }else{*/
      //detectLine();
      keepDistance();
      #ifdef ASSIGNMENT2
      if (motorDist_l < 0.1){
        startCrossingGap();
      }
      #else
      if (curstate == WAITTOSTART && motorDist_l < 0.1){
        curstate = STRAIGHTAHEAD;
      }
      #endif
      controlState();
#ifdef BOT_SERIAL
      //keepDistance();
#endif
      adjustMotor(motorLine_l*motorDist_l,motorLine_r*motorDist_r);
      //Serial.print("Motor line speed: ("); Serial.print(motorLine_l); Serial.print(", "); Serial.print(motorLine_r); Serial.println(")");
      //Serial.print("Motor dist speed: ("); Serial.print(motorDist_l); Serial.print(", "); Serial.print(motorDist_r); Serial.println(")");
    /*}*/
  /*}
  else
  {
      hearClap();
  }*/
}


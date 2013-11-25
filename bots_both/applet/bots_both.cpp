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

#include "WProgram.h"
void setup();
void loop();
void adjustMotor(float motor_l, float motor_r);
void hearClap();
void RGBLED(int r, int g, int b);
void startCrossingGap();
boolean isWaitingForGap();
void controlState();
void stopForOther();
void controlState();
void keepDistance();
void detectLine();
void speedChanged(float motor_l, float motor_r);
int getTotalDistance();
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
/*
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
*/
void adjustMotor(float motor_l, float motor_r){
  if (motor_l > 0){
    digitalWrite(ID_DIRECTION_L, HIGH);
    analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*motor_l)));
  }else{
    digitalWrite(ID_DIRECTION_L, LOW);
    analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*-motor_l)));
  }
  if (motor_r > 0){
    digitalWrite(ID_DIRECTION_R, HIGH);
    analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*motor_r)));
  }else{
    digitalWrite(ID_DIRECTION_R, LOW);
    analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*-motor_r)));
  }
  speedChanged(motor_l, motor_r);
}


void hearClap(){
  /* Select ADC_SOUND LEFT */
  digitalWrite(ID_FRONTREAR, LOW);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(50);
  sound_left = analogRead(ID_ADC_SOUND);

  if(sound_left <= CLAP_LOWER || sound_left >= CLAP_UPPER){
    move = true;
    lastSpeedChange = millis();
  }
}


void RGBLED(int r, int g, int b){
  analogWrite(ID_LED_RED, r);
  analogWrite(ID_LED_GREEN, g);
  analogWrite(ID_LED_BLUE, b);
}

#ifdef ASSIGNMENT2
void startCrossingGap()
{
  if (isWaitingForGap()){
    curstate = STARTCROSSGAP;
  }
}

boolean isWaitingForGap()
{
  if (curstate == WAITFORGAP && !cross_next_gap)
  {
    return true;
  }
  return false;
}

void controlState()
{
        int line_left, line_right;

        if (curstate == CONFIRMGAP){
          delay(200);
        }else if (curstate == CROSSGAP){
          delay(500);
        }else if (curstate == RETURNTOGAPSTART){
          delay(300);
        }else if(curstate == RETURNTOBENDSTART){
          delay(250);
        }
        
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
        
        Position curpos = ONTRACK;
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          curpos = OFFTRACK;
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          curpos = HALFTRACK;
        }
        
        
        // Start with 100% power for both motors
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
        State newstate = STRAIGHTAHEAD;
        if (curstate == STRAIGHTAHEAD || curstate == SMALLCORRECT || curstate == CROSSGAP){
          if (curpos == ONTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK){
            newstate = SMALLCORRECT;
          }else{
            newstate = CONFIRMGAP;
          }
        }else if (curstate == CONFIRMGAP){
          Serial.println("ConfirmGap");
          if (curpos != OFFTRACK){
            newstate = RETURNTOGAPSTART;
          }else{
            newstate = RETURNTOBENDSTART;
          }
        }else if (curstate == RETURNTOGAPSTART){
          newstate = WAITFORGAP;
        }else if (curstate == RETURNTOBENDSTART){
          newstate = LARGECORRECT;
          largecorrectsteps = 300;
        }else if (curstate == WAITFORGAP){
          if (cross_next_gap){
            newstate = CROSSGAP;
            cross_next_gap = false;
          }else{
            newstate = WAITFORGAP;
          }
        }else if (curstate == LARGECORRECT){
          if (curpos == ONTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK){
            newstate = SMALLCORRECT;
          }else{
            newstate = LARGECORRECT;
            largecorrectsteps--;
          }
        }else if (curstate = STARTCROSSGAP){
          cross_next_gap = true;
          newstate = CROSSGAP;
        }
        
        if (newstate == STRAIGHTAHEAD){
          motorLine_l = 1.0f;
          motorLine_r = 1.0f;
          RGBLED(0, 255, 0);
        }else if (newstate == SMALLCORRECT){
          if (ratio < 0){
            // The bot is seeing the line at the right side and is therefore heading too far to the left.
            // So the bot should turn right.
            //Serial.println("GOTO RIGHT");
            out_at = LEFT; // Keep track of where the line was last seen.
            // Decrease the speed of the right motor to go right.
            motorLine_r = 1.0f+LINE_FACTOR*ratio;
          }else{
            // The bot is seeing the line at the left side and is therefore heading too far to the right.
            // So the bot should turn left.
            //Serial.println("GOTO LEFT");
            out_at = RIGHT; // Keep track of where the line was last seen.
            // Decrease the speed of the left motor to go left.
            motorLine_l = 1.0f-LINE_FACTOR*ratio;
          }
          motorLine_r = motorLine_r*TURN_SPEED;
          motorLine_l = motorLine_l*TURN_SPEED;
          RGBLED(0, 0, 255);
        }else if (newstate == CONFIRMGAP){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 120, 120);
        }else if (newstate == RETURNTOGAPSTART || newstate == RETURNTOBENDSTART){
          motorLine_r = -1.0f;
          motorLine_l = -1.0f;
          Serial.println("Go Back!");
          RGBLED(0, 255, 100);
        }else if (newstate == WAITFORGAP){
          motorLine_r = 0.0f;
          motorLine_l = 0.0f;
          RGBLED(255, 0, 0);
        }else if (newstate == LARGECORRECT){
          if (largecorrectsteps > 200){
            motorLine_r = 0.8f;
            motorLine_l = -0.8f;
          }else if (largecorrectsteps > 0){
            motorLine_r = -0.8f;
            motorLine_l = 0.8f;
          }else if (largecorrectsteps > -100){
            motorLine_r = 0.8f;
            motorLine_l = -0.8f;
          }else if (largecorrectsteps > -150){
            motorLine_r = 0.8f;
            motorLine_l = 0.8f;
          }else{
            motorLine_r = 0.8f;
            motorLine_l = 0.8f;
            largecorrectsteps = 300;
          }
          RGBLED(255, 0, 100);
        }else if (newstate == CROSSGAP){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 255, 50);
        }
        
        curstate = newstate;
        
        motorLine_l = max(-1,motorLine_l);
        motorLine_r = max(-1,motorLine_r);
}
#else
void stopForOther()
{
  if (curstate != WAITTOSTART){
    curstate = STOPFOROTHER;
  }
}

void controlState()
{
        int line_left, line_right;

        if (curstate == CORRECTLEFT || curstate == CORRECTRIGHT){
          delay(600);
        }
        
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
        
        Position curpos = ONTRACK;
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          curpos = OFFTRACK;
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          curpos = HALFTRACK;
        }
        
        
        // Start with 100% power for both motors
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
        State newstate = STRAIGHTAHEAD;
        if (curstate == STRAIGHTAHEAD){
          if (curpos == OFFTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK && ratio > 0){
            newstate = CORRECTRIGHT;
          }else{
            newstate = CORRECTLEFT;
          }
        }else if (curstate == CORRECTLEFT || curstate == CORRECTRIGHT){
          newstate = STRAIGHTAHEAD;
        }else if (curstate == STOPFOROTHER){
          newstate = STOPFOROTHER;
        }
        
        if (newstate == STRAIGHTAHEAD){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 255, 0);
        }else if (newstate == CORRECTLEFT){
          motorLine_r = 0.8f;
          motorLine_l = -0.8f;
          RGBLED(0, 0, 255);
        }else if (newstate == CORRECTRIGHT){
          motorLine_r = -0.8f;
          motorLine_l = 0.8f;
          RGBLED(0, 0, 255);
        }else if (newstate == STOPFOROTHER){
          motorLine_r = 0.0f;
          motorLine_l = 0.0f;
          RGBLED(255, 0, 0);
        }
        
        curstate = newstate;
        
        motorLine_l = max(-1,motorLine_l);
        motorLine_r = max(-1,motorLine_r);
}
#endif


void keepDistance(){
  /* Select ADC_DISTANCE FRONT */
  digitalWrite(ID_FRONTREAR, HIGH);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(1);
  dist_front = analogRead(ID_ADC_DISTANCE);
  Serial.println(dist_front);
  if(dist_front <= DIST_THRESHOLD_CRITICAL){
    motorDist_l = 0;
    motorDist_r = 0;
    #ifdef ASSIGNMENT1
    stopForOther();
    #endif
  }else if(dist_front <= DIST_THRESHOLD){ //we see something
    motorDist_l *= 0.95; //slow down when we see something
    motorDist_r *= 0.95;
    #ifdef ASSIGNMENT1
    stopForOther();
    #endif
    //analogWrite(ID_LED_GREEN, 125);
    //analogWrite(ID_LED_BLUE, 125);
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
          //Serial.println("OFF TRACK");
          // Light up blue LED
          analogWrite(ID_LED_BLUE, 255);
          // Dim the green LED
          analogWrite(ID_LED_GREEN, 0);
          // We're off track!
          off_track = RECOVERY_STEPS;
          if (out_at == LEFT){
            // Try to rotate to the right
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motorLine_l = 0.9*TURN_SPEED; 
            motorLine_r = -TURN_SPEED;
          }else if (out_at == RIGHT){
            // Try to rotate to the left
            motorLine_l = -TURN_SPEED;
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motorLine_r = 0.9*TURN_SPEED;
          }else{
            motorLine_l = 1.0f; 
            motorLine_r = 1.0f;
          }
          off_track_count++;
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          off_track_count = 0;
          // The line is still visible at one side.
          if (off_track > 0){
            // The robot was recently off track, and should keep turning until it is better on the track.
            if (out_at == LEFT){
              // Try to rotate to the right
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_l = 0.9*TURN_SPEED; 
              motorLine_r = -TURN_SPEED;
            }else if (out_at == RIGHT){
              // Try to rotate to the left
              motorLine_l = -TURN_SPEED;
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_r = 0.9*TURN_SPEED;
            }else{
              motorLine_l = 1.0f; 
              motorLine_r = 1.0f;
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
              //Serial.println("GOTO RIGHT");
              out_at = LEFT; // Keep track of where the line was last seen.
              // Decrease the speed of the right motor to go right.
              motorLine_r = motorLine_r+LINE_FACTOR*ratio;
            }else{
              // The bot is seeing the line at the left side and is therefore heading too far to the right.
              // So the bot should turn left.
              Serial.println("GOTO LEFT");
              out_at = RIGHT; // Keep track of where the line was last seen.
              // Decrease the speed of the left motor to go left.
              motorLine_l = motorLine_l-LINE_FACTOR*ratio;
            }
            // Decrease the speed a little so the bot doesn't miss the corner.
            motorLine_l = motorLine_l*TURN_SPEED;
            motorLine_r = motorLine_r*TURN_SPEED;
          }
          //Serial.print("Correction: "); Serial.print(motorLine_l); Serial.print("; "); Serial.println(motorLine_r);
        }else{
          // The bot is heading in the correct direction! No adjustments have to be made.
          off_track = 0;
          off_track_count = 0;
          out_at = STRAIGHT;
          // Just light up the green light to show everything is okay :)
          analogWrite(ID_LED_BLUE, 0);
          analogWrite(ID_LED_GREEN, 255);
        }
        motorLine_l = max(-1,motorLine_l);
        motorLine_r = max(-1,motorLine_r);
}

#define DISTANCE_CONVERSION_FACTOR 0.001f

void speedChanged(float motor_l, float motor_r){
  // Don't measure off track distance
  if (off_track == 0){
    unsigned long newtime = millis();
    measuredDistance += ((lCurrentSpd+rCurrentSpd)/2) * (newtime-lastSpeedChange);
    lastSpeedChange = newtime;
    
    lCurrentSpd = motor_l;
    rCurrentSpd = motor_r;
  }
}

int getTotalDistance(){
  return (int)(measuredDistance*DISTANCE_CONVERSION_FACTOR);
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}


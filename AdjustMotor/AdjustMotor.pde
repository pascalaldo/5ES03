#define ID_ENCODER_R      2
#define ID_ENCODER_L      4
#define ID_SPEED_R        5
#define ID_SPEED_L        6
#define ID_DIRECTION_R    7
#define ID_DIRECTION_L    8

#define DELAY_STOP      200
#define DELAY_DRIVE     1500

#include <math.h> //include math functions

static float lCurrentSpd = 0;
static float rCurrentSpd = 0;

void setup()
{
        pinMode(ID_DIRECTION_L, OUTPUT);
        digitalWrite(ID_DIRECTION_L, HIGH); 
        pinMode(ID_DIRECTION_R, OUTPUT);
        digitalWrite(ID_DIRECTION_R, HIGH);
        
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        

}


/*
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
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_L, HIGH);
  }
  
  if (lCurrentSpd >=0 && lSpd <0) //left going forward and want bacwkard
  { 
        analogWrite(ID_SPEED_L, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_L, LOW);
  }

  if (rCurrentSpd <=0 && rSpd >0) //right going backward and want forward
  { 
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_R, HIGH);
  }
  if (rCurrentSpd >=0 && rSpd <0) //right going forward and want backward
  { 
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP); //need to stop a bit
        digitalWrite(ID_DIRECTION_R, LOW);
  }

  lCurrentSpd = lSpd;
  rCurrentSpd = rSpd;

  lSpd = abs(lSpd*255);
  rSpd = abs(rSpd*255);

 //TODO: insert final speed function here
 
}




void loop()
{
        static int speed_left  = 200;
        static int speed_right = 2;

        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP);
        digitalWrite(ID_DIRECTION_L, HIGH);
        digitalWrite(ID_DIRECTION_R, HIGH);
        analogWrite(ID_SPEED_L, speed_left);
        analogWrite(ID_SPEED_R, speed_right);
        delay(DELAY_DRIVE);

        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);
        delay(DELAY_STOP);
        digitalWrite(ID_DIRECTION_L, LOW);
        digitalWrite(ID_DIRECTION_R, LOW);
        analogWrite(ID_SPEED_L, speed_left);
        analogWrite(ID_SPEED_R, speed_right);
        delay(DELAY_DRIVE);

        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);

       
}

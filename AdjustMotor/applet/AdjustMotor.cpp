#define ID_ENCODER_R      2
#define ID_ENCODER_L      4
#define ID_SPEED_R        5
#define ID_SPEED_L        6
#define ID_DIRECTION_R    7
#define ID_DIRECTION_L    8

#define DELAY_STOP      500
#define DELAY_DRIVE     1000

#include <math.h> //include math functions

#define hasSerial     true

#include "WProgram.h"
void setup();
void adjustMotor(float lSpd, float rSpd);
void loop();
static float lCurrentSpd = 0;
static float rCurrentSpd = 0;

void setup()
{
  Serial.begin(57600);
        pinMode(ID_DIRECTION_L, OUTPUT);
        digitalWrite(ID_DIRECTION_L, HIGH); 
        pinMode(ID_DIRECTION_R, OUTPUT);
        digitalWrite(ID_DIRECTION_R, HIGH);
        
        analogWrite(ID_SPEED_L, 0);
        analogWrite(ID_SPEED_R, 0);

}


/*
@pre: -0.6 <= lSpd <=0.6 due to engine not engaging otherwise
@pre: -0.6 <= rSpd <=0.6 due to engine not engaging otherwise
@pre: 
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
  rSpd = abs((int) (rSpd*(hasSerial ? 252 : 255))); //get correction

  int s1 = (int) lSpd;
  analogWrite(ID_SPEED_L,s1);

  int s2 = (int) rSpd;
  analogWrite(ID_SPEED_R,s2);


 
}




void loop()
{

 
        adjustMotor(-(0.65),-(0.65));
        delay(DELAY_DRIVE);      
        adjustMotor(0.65,0.65);
        delay(DELAY_DRIVE);      
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}


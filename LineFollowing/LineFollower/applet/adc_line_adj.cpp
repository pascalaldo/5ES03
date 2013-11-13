#define ID_ADC_LINE  2

#define ID_SPEED_R       5
#define ID_SPEED_L       6
#define ID_DIRECTION_R   7
#define ID_DIRECTION_L   8
#define ID_LEFTRIGHT    12
#define ID_FRONTREAR    13

#define line_sensor_R_max 570
#define line_sensor_R_min  75
#define line_sensor_L_max 480
#define line_sensor_L_min  85

#define Kp  0.3
#define Ki  0.001
#define Kd  0.6

#define RIGHT_REF_SPEED  200
#define LEFT_REF_SPEED   200

#define DELAY_VAL	 10
#define DELAY_STOP      200
#define DELAY_DRIVE    1000

#include <math.h> //include math functions

#define hasSerial     true

#include "WProgram.h"
void setup();
void line_following();
void loop();
static float lCurrentSpd = 0;
static float rCurrentSpd = 0;
static float lineSpeedR = 0;
static float lineSpeedL = 0;

void setup()
{
	Serial.begin(57600);
	Serial.println("ADC_LINE");

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
}

void line_following() 
{
	int line_left, line_right;

	/* Select ADC_LINE LEFT */
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, LOW);
        delay(1); /* Give ADC time to sample */
	line_left = analogRead(ID_ADC_LINE);

	/* Select ADC_LINE RIGHT */
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, HIGH);
        delay(1); /* Give ADC time to sample */
	line_right = analogRead(ID_ADC_LINE);
        
        float error; 
        float integral = 0;
        float derivative = 0;
        float motorRatio = 0;
        float lasterror = 0;
        int motorSpeed = 0;
        
        if (line_right<500) {
          error = ( line_sensor_R_max-(float)line_right )/(line_sensor_R_max-line_sensor_R_min);
        }
        else if (line_left<500) {
          error = -( line_sensor_L_max-(float)line_left )/(line_sensor_L_max-line_sensor_L_min);
        }
        else {
          error = 0.0;
        }
        
        integral = integral+error*DELAY_VAL;
        derivative = (error-lasterror)/DELAY_VAL;
        motorRatio = Kp*error+Ki*integral+Kd*derivative;
        motorSpeed = motorRatio*200;
        lasterror = error;
        
        lineSpeedR = RIGHT_REF_SPEED+motorSpeed;
        lineSpeedL = LEFT_REF_SPEED-motorSpeed;
}

void loop()
{

        line_following();
        //analogWrite(ID_SPEED_L,LEFT_REF_SPEED+motorSpeed);
        //analogWrite(ID_SPEED_R,RIGHT_REF_SPEED-motorSpeed);
        
        analogWrite(ID_SPEED_L,lineSpeedL);
        analogWrite(ID_SPEED_R,lineSpeedR);
        
        /* Serial.print("ADC_LINE L/R ");
        Serial.print("Left: "); Serial.print(line_left);  Serial.print(" ");
        Serial.print("Right: "); Serial.print(line_right); Serial.println(" ");
        Serial.print("Error: "); Serial.print(error); Serial.print(" ");
        Serial.print("Right Motor: "); Serial.print(RIGHT_REF_SPEED+motorSpeed); Serial.print(" ");
        Serial.print("Left Motor: "); Serial.print(LEFT_REF_SPEED-motorSpeed); Serial.print(" "); */

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


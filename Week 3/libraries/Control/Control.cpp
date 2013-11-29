#include <defs.h>
#include "Arduino.h"
#include <Control.h>

Control::Control()
{
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
   
void Control::keepDistance(){
 /* Select ADC_DISTANCE FRONT */
 digitalWrite(ID_FRONTREAR, HIGH);
 digitalWrite(ID_LEFTRIGHT, LOW);
 delay(1);
 distanceFront = analogRead(ID_ADC_DISTANCE);
 
 //Serial.println(dist_front);
 if(distanceFront <= DIST_THRESHOLD_CRITICAL){
   motorDist_l = 0;
   motorDist_r = 0;
 }else if(distanceFront <= DIST_THRESHOLD){ //we see something
   motorDist_l *= 0.95; //slow down when we see something
   motorDist_r *= 0.95;
 }
 else //not seeing anything
 {
   motorDist_l = 1;
   motorDist_r = 1;
 }
}

void Control::lookToSides(){
 /* Select ADC_DISTANCE FRONT */
 digitalWrite(ID_FRONTREAR, LOW);
 digitalWrite(ID_LEFTRIGHT, LOW);
 delay(1);
 distanceLeft = analogRead(ID_ADC_DISTANCE);
 
  /* Select ADC_DISTANCE FRONT */
 digitalWrite(ID_FRONTREAR, LOW);
 digitalWrite(ID_LEFTRIGHT, HIGH);
 delay(1);
 distanceRight = analogRead(ID_ADC_DISTANCE);
}
   
void Control::adjustMotor(float motor_l, float motor_r){
  if (((motor_l >= 0) != (currentSpeed_l >= 0)) ||
      ((motor_r >= 0) != (currentSpeed_r >= 0))){
    analogWrite(ID_SPEED_L, 0);
    analogWrite(ID_SPEED_R, 0);
    delay(MOTOR_CORRECTION_DELAY);
  }
  
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
  //speedChanged(motor_l, motor_r);
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

  ratio = ((float)(lineCorrected_l-lineCorrected_r)/1000.0f);
  
  //Serial.print("Ratio: "); Serial.println(ratio);
  
  currentPosition = ControlEnums::ONTRACK;
  if (lineCorrected_l < LINE_THRESHOLD && lineCorrected_r < LINE_THRESHOLD){
    currentPosition = ControlEnums::OFFTRACK;
  }else if (lineCorrected_l < LINE_THRESHOLD || lineCorrected_r < LINE_THRESHOLD){
    currentPosition = ControlEnums::HALFTRACK;
  }
}

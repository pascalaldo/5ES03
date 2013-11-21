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


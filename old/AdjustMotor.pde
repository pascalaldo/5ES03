#define MOTOR_CORRECTION_DELAY 100
#ifdef NOTDEFINED
void adjustMotor(float motor_l, float motor_r){
  if (((motor_l >= 0) != (lCurrentSpd >= 0)) ||
      ((motor_r >= 0) != (rCurrentSpd >= 0))){
    analogWrite(ID_SPEED_L, 0);
    analogWrite(ID_SPEED_R, 0);
    delay(MOTOR_CORRECTION_DELAY);
  }
  
  if (motor_l >= 0){
    if (lCurrentSpd < 0){
      digitalWrite(ID_DIRECTION_L, HIGH);
    }
    analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*motor_l)));
  }else{
    if (lCurrentSpd >= 0){
      digitalWrite(ID_DIRECTION_L, LOW);
    }
    analogWrite(ID_SPEED_L, ((int)(MOTOR_CORRECTION_L*-motor_l)));
  }
  if (motor_r >= 0){
    if (rCurrentSpd < 0){
      digitalWrite(ID_DIRECTION_R, HIGH);
    }
    analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*motor_r)));
  }else{
    if (rCurrentSpd >= 0){
      digitalWrite(ID_DIRECTION_R, LOW);
    }
    analogWrite(ID_SPEED_R, ((int)(MOTOR_CORRECTION_R*-motor_r)));
  }
  speedChanged(motor_l, motor_r);
}
#endif

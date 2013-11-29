#define DISTANCE_CONVERSION_FACTOR 0.001f
#ifdef NOTDEFINED
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
#endif

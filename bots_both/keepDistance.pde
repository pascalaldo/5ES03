#ifdef BOT_SERIAL
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
  }else if(dist_front <= DIST_THRESHOLD){ //we see something
    motorDist_l *= 0.95; //slow down when we see something
    motorDist_r *= 0.95;
    //analogWrite(ID_LED_GREEN, 125);
    //analogWrite(ID_LED_BLUE, 125);
  }
  else //not seeing anything
  {
    motorDist_l = 1;
    motorDist_r = 1;
  }
  
}
#endif

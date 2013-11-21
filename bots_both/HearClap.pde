
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



void measureDistance() 
{
  float loopTime, lastLoopTime;
  float distance; 
  
  Serial.begin(57600);
  loopTime = millis()/1000;  // get the number of seconds since the program started
  distance = distance + 0.5*(abs(lCurrentSpd+rCurrentSpd))*((float)loopTime-(float)lastLoopTime);
  lastLoopTime = loopTime;
  Serial.print(distance); Serial.print(" ");
}

#define ASSIGNMENT1

#include <defs.h>
#include <Control.h>
#include <Control1.h>
#include <Control2.h>

#ifdef ASSIGNMENT1
Control1 control;
#else
Control2 control;
#endif

void setup(){  
  #ifdef DEBUG
  Serial.begin(57600); //For debugging purposes. Ignore if not needed.
  #endif
  
  pinMode(ID_LEFTRIGHT, OUTPUT);
  digitalWrite(ID_LEFTRIGHT, HIGH);

  pinMode(ID_FRONTREAR, OUTPUT);
  digitalWrite(ID_FRONTREAR, HIGH);
  
  /*
   * These two should not be necessary, but are.
   * If they are left out the sensor readings
   * are incorrect.
   * Copied without shame.
   */
  pinMode(ID_DIRECTION_R, OUTPUT);
  digitalWrite(ID_DIRECTION_R, HIGH);

  pinMode(ID_DIRECTION_L, OUTPUT);
  digitalWrite(ID_DIRECTION_L, HIGH);
}

void loop(){
  control.doLoop();
}


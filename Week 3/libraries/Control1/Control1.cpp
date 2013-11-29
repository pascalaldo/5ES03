#include <defs.h>
#include "Arduino.h"
#include "Control1.h"

Control1::Control1()
 {
   currentState = ControlEnums::STRAIGHTAHEAD;
   positionOther = ControlEnums::STRAIGHT;
   _hasLight = false;
   setHasLight(false);
 }
 
bool Control1::hasLight()
 {
   return _hasLight;
 }
 
void Control1::setHasLight(bool value)
 {
   if (value != _hasLight)
   {
       for (int i = 0; i < 256; i++)
       {
           setRGBLED(0, (_hasLight ? (255-i) : i), 0);
           delay(10);
       }
       _hasLight = value;
   }
 }
 
void Control1::doLoop()
 {
   keepDistance();
   lookToSides();
   if (currentState == ControlEnums::WAITTOSTART && motorDist_l < 0.1){
     currentState = ControlEnums::STRAIGHTAHEAD;
   }
   if (distanceFront <= DIST_THRESHOLD){
	stopForOther(ControlEnums::STRAIGHT);
   }else if (distanceRight <= DIST_THRESHOLD){
    stopForOther(ControlEnums::RIGHT);
   }/*else if (distanceLeft <= DIST_THRESHOLD){
    stopForOther(ControlEnums::LEFT);
   }*/
   // TODO NETWORKING: Connect signal for the transfer of the light
   if (distanceLeft <= DIST_THRESHOLD){
	transferLight();
   }
   updateState();
   adjustMotor(motorLine_l*motorDist_l,motorLine_r*motorDist_r);
 }
   
void Control1::stopForOther(ControlEnums::Direction pos)
 {
   if (currentState != ControlEnums::WAITTOSTART){
     currentState = ControlEnums::STOPFOROTHER;
     positionOther = pos;
     // TODO NETWORKING: Send out signal that robot has seen the other (at pos)
   }
 }
 
void Control1::transferLight()
 {
   if (currentState == ControlEnums::STOPFOROTHER){
     currentState = ControlEnums::STARTTRANSFERLIGHT;
   }
 }
   
void Control1::updateState()
 {
  if (currentState == ControlEnums::CORRECTLEFT || currentState == ControlEnums::CORRECTRIGHT){
    delay(600);
  }
  
  readSensors();
  
  // Start with 100% power for both motors
  motorLine_l = 1.0f;
  motorLine_r = 1.0f;
  
  ControlEnums::State newstate = ControlEnums::STRAIGHTAHEAD;
  if (currentState == ControlEnums::STRAIGHTAHEAD){
    if (currentPosition == ControlEnums::OFFTRACK){
      newstate = ControlEnums::STRAIGHTAHEAD;
    }else if (currentPosition == ControlEnums::HALFTRACK && ratio > 0){
      newstate = ControlEnums::CORRECTRIGHT;
    }else{
      newstate = ControlEnums::CORRECTLEFT;
    }
  }else if (currentState == ControlEnums::CORRECTLEFT || currentState == ControlEnums::CORRECTRIGHT){
    newstate = ControlEnums::STRAIGHTAHEAD;
  }else if (currentState == ControlEnums::STOPFOROTHER){
    newstate = ControlEnums::STOPFOROTHER;
  }else if (currentState == ControlEnums::STARTTRANSFERLIGHT){
    newstate = ControlEnums::TRANSFERLIGHT;
  }else if (currentState == ControlEnums::TRANSFERLIGHT){
    if (positionOther == ControlEnums::RIGHT){
        newstate = ControlEnums::CORRECTLEFT;
    }else{
        newstate = ControlEnums::CORRECTRIGHT;
    }
  }
  
  if (newstate == ControlEnums::STRAIGHTAHEAD){
    motorLine_r = 1.0f;
    motorLine_l = 1.0f;
#ifdef DEBUG
    setRGBLED(0, 255, 0);
#endif
  }else if (newstate == ControlEnums::CORRECTLEFT){
    motorLine_r = 0.8f;
    motorLine_l = -0.8f;
#ifdef DEBUG
    setRGBLED(0, 0, 255);
#endif DEBUG
  }else if (newstate == ControlEnums::CORRECTRIGHT){
    motorLine_r = -0.8f;
    motorLine_l = 0.8f;
#ifdef DEBUG
    setRGBLED(0, 0, 255);
#endif
  }else if (newstate == ControlEnums::STOPFOROTHER){
    motorLine_r = 0.0f;
    motorLine_l = 0.0f;
#ifdef DEBUG
    setRGBLED(255, 0, 0);
#endif
  }else if (newstate == ControlEnums::TRANSFERLIGHT){
    setHasLight(!hasLight());
    motorLine_r = 0.0f;
    motorLine_l = 0.0f;
  }
  
  currentState = newstate;
  
  motorLine_l = max(-1,motorLine_l);
  motorLine_r = max(-1,motorLine_r);
 }

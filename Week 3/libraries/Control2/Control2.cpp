#include <defs.h>
#include "Arduino.h"
#include "Control2.h"

Control2::Control2()
 {
   out_at = ControlEnums::STRAIGHT;
   cross_next_gap = false;
   largecorrectsteps = 300;
   currentState = ControlEnums::STRAIGHTAHEAD;
 }
   
 void Control2::doLoop()
 {
   keepDistance();
   // TODO NETWORKING: Connect to signal for crossing the gap.
   if (motorDist_l < 0.1){
     startCrossingGap();
   }
   updateState();
   // TODO uncomment this, is commented because distance sensors are now used as replacement for networking
   adjustMotor(motorLine_l/**motorDist_l*/,motorLine_r/**motorDist_r*/);
 }
   
 void Control2::startCrossingGap()
 {
   if (isWaitingForGap()){
     currentState = ControlEnums::STARTCROSSGAP;
   }
 }
   
 bool Control2::isWaitingForGap()
 {
   return (currentState == ControlEnums::WAITFORGAP && !cross_next_gap);
 }
   
 void Control2::updateState()
 {
  int line_left, line_right;

  if (currentState == ControlEnums::CONFIRMGAP){
    delay(200);
  }else if (currentState == ControlEnums::CROSSGAP){
    delay(300);
  }else if (currentState == ControlEnums::RETURNTOGAPSTART){
    delay(200);
  }else if(currentState == ControlEnums::RETURNTOBENDSTART){
    delay(250);
  }
  
  readSensors();
  
  // Start with 100% power for both motors
  motorLine_l = 1.0f;
  motorLine_r = 1.0f;
  
  ControlEnums::State newstate = ControlEnums::STRAIGHTAHEAD;
  if (currentState == ControlEnums::STRAIGHTAHEAD || currentState == ControlEnums::SMALLCORRECT || currentState == ControlEnums::CROSSGAP){
    if (currentPosition == ControlEnums::ONTRACK){
      newstate = ControlEnums::STRAIGHTAHEAD;
    }else if (currentPosition == ControlEnums::HALFTRACK){
      newstate = ControlEnums::SMALLCORRECT;
    }else{
      newstate = ControlEnums::CONFIRMGAP;
    }
  }else if (currentState == ControlEnums::CONFIRMGAP){
    //Serial.println("ConfirmGap");
    if (currentPosition != ControlEnums::OFFTRACK){
      if (cross_next_gap){
		  if (currentPosition == ControlEnums::HALFTRACK){
			newstate = ControlEnums::SMALLCORRECT;
		  }else{
			newstate = ControlEnums::STRAIGHTAHEAD;
		  }
		  cross_next_gap = false;
	  }else{
		newstate = ControlEnums::RETURNTOGAPSTART;
	  }
    }else{
      newstate = ControlEnums::RETURNTOBENDSTART;
    }
  }else if (currentState == ControlEnums::RETURNTOGAPSTART){
    newstate = ControlEnums::WAITFORGAP;
    // TODO Networking: Send signal that robot is waiting
  }else if (currentState == ControlEnums::RETURNTOBENDSTART){
    newstate = ControlEnums::LARGECORRECT;
    largecorrectsteps = 300;
  }else if (currentState == ControlEnums::WAITFORGAP){
      newstate = ControlEnums::WAITFORGAP;
  }else if (currentState == ControlEnums::LARGECORRECT){
    if (currentPosition == ControlEnums::ONTRACK){
      newstate = ControlEnums::STRAIGHTAHEAD;
    }else if (currentPosition == ControlEnums::HALFTRACK){
      newstate = ControlEnums::SMALLCORRECT;
    }else{
      newstate = ControlEnums::LARGECORRECT;
      largecorrectsteps--;
    }
  }else if (currentState = ControlEnums::STARTCROSSGAP){
    cross_next_gap = true;
    newstate = ControlEnums::CROSSGAP;
  }
  
  if (newstate == ControlEnums::STRAIGHTAHEAD){
    motorLine_l = 1.0f;
    motorLine_r = 1.0f;
    out_at = ControlEnums::STRAIGHT;
    setRGBLED(0, 255, 0);
  }else if (newstate == ControlEnums::SMALLCORRECT){
    if (ratio < 0){
      // The bot is seeing the line at the right side and is therefore heading too far to the left.
      // So the bot should turn right.
      //Serial.println("GOTO RIGHT");
      out_at = ControlEnums::LEFT; // Keep track of where the line was last seen.
      // Decrease the speed of the right motor to go right.
      motorLine_r = 1.0f+LINE_FACTOR*ratio;
      motorLine_r = max(motorLine_r, 0);
    }else{
      // The bot is seeing the line at the left side and is therefore heading too far to the right.
      // So the bot should turn left.
      //Serial.println("GOTO LEFT");
      out_at = ControlEnums::RIGHT; // Keep track of where the line was last seen.
      // Decrease the speed of the left motor to go left.
      motorLine_l = 1.0f-LINE_FACTOR*ratio;
      motorLine_l = max(motorLine_l, 0);
    }
    motorLine_r = motorLine_r*TURN_SPEED;
    motorLine_l = motorLine_l*TURN_SPEED;
    setRGBLED(0, 0, 255);
  }else if (newstate == ControlEnums::CONFIRMGAP){
    motorLine_r = 1.0f;
    motorLine_l = 1.0f;
    setRGBLED(0, 120, 120);
  }else if (newstate == ControlEnums::RETURNTOGAPSTART || newstate == ControlEnums::RETURNTOBENDSTART){
    motorLine_r = -1.0f;
    motorLine_l = -1.0f;
    //Serial.println("Go Back!");
    setRGBLED(0, 255, 100);
  }else if (newstate == ControlEnums::WAITFORGAP){
    motorLine_r = 0.0f;
    motorLine_l = 0.0f;
    setRGBLED(255, 0, 0);
  }else if (newstate == ControlEnums::LARGECORRECT){
    if (largecorrectsteps > 200){
      motorLine_r = 0.8f;
      motorLine_l = -0.8f;
    }else if (largecorrectsteps > 0){
      motorLine_r = -0.8f;
      motorLine_l = 0.8f;
    }else if (largecorrectsteps > -100){
      motorLine_r = 0.8f;
      motorLine_l = -0.8f;
    }else if (largecorrectsteps > -150){
      motorLine_r = 0.8f;
      motorLine_l = 0.8f;
    }else{
      motorLine_r = 0.8f;
      motorLine_l = 0.8f;
      largecorrectsteps = 300;
    }
    if (out_at == ControlEnums::LEFT){
		motorLine_r *= -1.0f;
		motorLine_l *= -1.0f;
	}
    setRGBLED(255, 0, 100);
  }else if (newstate == ControlEnums::CROSSGAP){
    motorLine_r = 1.0f;
    motorLine_l = 1.0f;
    setRGBLED(0, 255, 50);
  }
  
  currentState = newstate;
  
  motorLine_l = max(-1,motorLine_l);
  motorLine_r = max(-1,motorLine_r);
}

#ifndef CONTROL1_H
#define CONTROL1_H

#include <Control.h>
 
  class Control1 : public Control
 {
   public:
   Control1();
   void doLoop();
   void updateState();
   bool hasLight();
   void setHasLight(bool value);
   
   private:
   void stopForOther(ControlEnums::Direction pos);
   void transferLight();
   ControlEnums::Direction positionOther;
   bool _hasLight;
 };
 
#endif

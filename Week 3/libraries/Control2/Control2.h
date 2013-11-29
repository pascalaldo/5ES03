#ifndef CONTROL2_H
#define CONTROL2_H

#include <Control.h>

  class Control2 : public Control
 {
   public:
   Control2();
   void doLoop();
   void startCrossingGap();
   bool isWaitingForGap();
   void updateState();

   private:
   ControlEnums::Direction out_at;
   bool cross_next_gap;
   int largecorrectsteps;
 };

#endif

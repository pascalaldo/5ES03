void RGBLED(int r, int g, int b){
  analogWrite(ID_LED_RED, r);
  analogWrite(ID_LED_GREEN, g);
  analogWrite(ID_LED_BLUE, b);
}

#ifdef ASSIGNMENT2
void startCrossingGap()
{
  if (isWaitingForGap()){
    curstate = STARTCROSSGAP;
  }
}

boolean isWaitingForGap()
{
  if (curstate == WAITFORGAP && !cross_next_gap)
  {
    return true;
  }
  return false;
}

void controlState()
{
        int line_left, line_right;

        if (curstate == CONFIRMGAP){
          delay(200);
        }else if (curstate == CROSSGAP){
          delay(500);
        }else if (curstate == RETURNTOGAPSTART){
          delay(300);
        }else if(curstate == RETURNTOBENDSTART){
          delay(250);
        }
        
	// Read out the left line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, LOW);
        delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_left = analogRead(ID_ADC_LINE);

	// Read out the right line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, HIGH);
        delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_right = analogRead(ID_ADC_LINE);

        // Correct for bot dependent anomalties
        float line_l_cor = CORRECTION_LEFT*(float)line_left;
        float line_r_cor = CORRECTION_RIGHT*(float)line_right;

        //Serial.print("Left: "); Serial.println(line_l_cor);
        //Serial.print("Right: "); Serial.println(line_r_cor);

        float ratio = ((float)(line_l_cor-line_r_cor)/1000.0f);
        
        //Serial.print("Ratio: "); Serial.println(ratio);
        
        Position curpos = ONTRACK;
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          curpos = OFFTRACK;
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          curpos = HALFTRACK;
        }
        
        
        // Start with 100% power for both motors
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
        State newstate = STRAIGHTAHEAD;
        if (curstate == STRAIGHTAHEAD || curstate == SMALLCORRECT || curstate == CROSSGAP){
          if (curpos == ONTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK){
            newstate = SMALLCORRECT;
          }else{
            newstate = CONFIRMGAP;
          }
        }else if (curstate == CONFIRMGAP){
          Serial.println("ConfirmGap");
          if (curpos != OFFTRACK){
            newstate = RETURNTOGAPSTART;
          }else{
            newstate = RETURNTOBENDSTART;
          }
        }else if (curstate == RETURNTOGAPSTART){
          newstate = WAITFORGAP;
        }else if (curstate == RETURNTOBENDSTART){
          newstate = LARGECORRECT;
          largecorrectsteps = 300;
        }else if (curstate == WAITFORGAP){
          if (cross_next_gap){
            newstate = CROSSGAP;
            cross_next_gap = false;
          }else{
            newstate = WAITFORGAP;
          }
        }else if (curstate == LARGECORRECT){
          if (curpos == ONTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK){
            newstate = SMALLCORRECT;
          }else{
            newstate = LARGECORRECT;
            largecorrectsteps--;
          }
        }else if (curstate = STARTCROSSGAP){
          cross_next_gap = true;
          newstate = CROSSGAP;
        }
        
        if (newstate == STRAIGHTAHEAD){
          motorLine_l = 1.0f;
          motorLine_r = 1.0f;
          RGBLED(0, 255, 0);
        }else if (newstate == SMALLCORRECT){
          if (ratio < 0){
            // The bot is seeing the line at the right side and is therefore heading too far to the left.
            // So the bot should turn right.
            //Serial.println("GOTO RIGHT");
            out_at = LEFT; // Keep track of where the line was last seen.
            // Decrease the speed of the right motor to go right.
            motorLine_r = 1.0f+LINE_FACTOR*ratio;
          }else{
            // The bot is seeing the line at the left side and is therefore heading too far to the right.
            // So the bot should turn left.
            //Serial.println("GOTO LEFT");
            out_at = RIGHT; // Keep track of where the line was last seen.
            // Decrease the speed of the left motor to go left.
            motorLine_l = 1.0f-LINE_FACTOR*ratio;
          }
          motorLine_r = motorLine_r*TURN_SPEED;
          motorLine_l = motorLine_l*TURN_SPEED;
          RGBLED(0, 0, 255);
        }else if (newstate == CONFIRMGAP){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 120, 120);
        }else if (newstate == RETURNTOGAPSTART || newstate == RETURNTOBENDSTART){
          motorLine_r = -1.0f;
          motorLine_l = -1.0f;
          Serial.println("Go Back!");
          RGBLED(0, 255, 100);
        }else if (newstate == WAITFORGAP){
          motorLine_r = 0.0f;
          motorLine_l = 0.0f;
          RGBLED(255, 0, 0);
        }else if (newstate == LARGECORRECT){
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
          RGBLED(255, 0, 100);
        }else if (newstate == CROSSGAP){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 255, 50);
        }
        
        curstate = newstate;
        
        motorLine_l = max(-1,motorLine_l);
        motorLine_r = max(-1,motorLine_r);
}
#else
void stopForOther()
{
  if (curstate != WAITTOSTART){
    curstate = STOPFOROTHER;
  }
}

void controlState()
{
        int line_left, line_right;

        if (curstate == CORRECTLEFT || curstate == CORRECTRIGHT){
          delay(600);
        }
        
	// Read out the left line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, LOW);
        delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_left = analogRead(ID_ADC_LINE);

	// Read out the right line detector
	digitalWrite(ID_FRONTREAR, LOW);
	digitalWrite(ID_LEFTRIGHT, HIGH);
        delay(DELAY_READ_LINE); /* Give ADC time to sample */
	line_right = analogRead(ID_ADC_LINE);

        // Correct for bot dependent anomalties
        float line_l_cor = CORRECTION_LEFT*(float)line_left;
        float line_r_cor = CORRECTION_RIGHT*(float)line_right;

        //Serial.print("Left: "); Serial.println(line_l_cor);
        //Serial.print("Right: "); Serial.println(line_r_cor);

        float ratio = ((float)(line_l_cor-line_r_cor)/1000.0f);
        
        //Serial.print("Ratio: "); Serial.println(ratio);
        
        Position curpos = ONTRACK;
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          curpos = OFFTRACK;
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          curpos = HALFTRACK;
        }
        
        
        // Start with 100% power for both motors
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
        State newstate = STRAIGHTAHEAD;
        if (curstate == STRAIGHTAHEAD){
          if (curpos == OFFTRACK){
            newstate = STRAIGHTAHEAD;
          }else if (curpos == HALFTRACK && ratio > 0){
            newstate = CORRECTRIGHT;
          }else{
            newstate = CORRECTLEFT;
          }
        }else if (curstate == CORRECTLEFT || curstate == CORRECTRIGHT){
          newstate = STRAIGHTAHEAD;
        }else if (curstate == STOPFOROTHER){
          newstate = STOPFOROTHER;
        }
        
        if (newstate == STRAIGHTAHEAD){
          motorLine_r = 1.0f;
          motorLine_l = 1.0f;
          RGBLED(0, 255, 0);
        }else if (newstate == CORRECTLEFT){
          motorLine_r = 0.8f;
          motorLine_l = -0.8f;
          RGBLED(0, 0, 255);
        }else if (newstate == CORRECTRIGHT){
          motorLine_r = -0.8f;
          motorLine_l = 0.8f;
          RGBLED(0, 0, 255);
        }else if (newstate == STOPFOROTHER){
          motorLine_r = 0.0f;
          motorLine_l = 0.0f;
          RGBLED(255, 0, 0);
        }
        
        curstate = newstate;
        
        motorLine_l = max(-1,motorLine_l);
        motorLine_r = max(-1,motorLine_r);
}
#endif


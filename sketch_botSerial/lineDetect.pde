void detectLine()
{
        int line_left, line_right;

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
        
        // Start with 100% power for both motors
        motorLine_l = 1.0f;
        motorLine_r = 1.0f;
        
        if (line_l_cor < LINE_THRESHOLD && line_r_cor < LINE_THRESHOLD){
          // If both values are below the threshold, the bot is off track.
          // Try desperately rotating to get back on the track.
          Serial.println("OFF TRACK");
          // Light up blue LED
          analogWrite(ID_LED_BLUE, 255);
          // Dim the green LED
          analogWrite(ID_LED_GREEN, 0);
          // We're off track!
          off_track = RECOVERY_STEPS;
          if (out_at_left){
            // Try to rotate to the right
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motorLine_l = 0.9*TURN_SPEED; 
            motorLine_r = -TURN_SPEED;
          }else{
            // Try to rotate to the left
            motorLine_l = -TURN_SPEED;
            // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
            motorLine_r = 0.9*TURN_SPEED;
          }
        }else if (line_l_cor < LINE_THRESHOLD || line_r_cor < LINE_THRESHOLD){
          // The line is still visible at one side.
          if (off_track > 0){
            // The robot was recently off track, and should keep turning until it is better on the track.
            if (out_at_left){
              // Try to rotate to the right
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_l = 0.9*TURN_SPEED; 
              motorLine_r = -TURN_SPEED;
            }else{
              // Try to rotate to the left
              motorLine_l = -TURN_SPEED;
              // Don't use full speed here, it might be more efficient to not rotate around the axis of the robot
              motorLine_r = 0.9*TURN_SPEED;
            }
            off_track--;
          }else{
            // The bot is somewhat off the track, adjust the direction a little.
            // Dim all LEDS
            analogWrite(ID_LED_BLUE, 0);
            analogWrite(ID_LED_GREEN, 0);
            
            // Which direction should we go to?
            if (ratio < 0){
              // The bot is seeing the line at the right side and is therefore heading too far to the left.
              // So the bot should turn right.
              Serial.println("GOTO RIGHT");
              out_at_left = true; // Keep track of where the line was last seen.
              // Decrease the speed of the right motor to go right.
              motorLine_l = motorLine_l-LINE_FACTOR*ratio;
            }else{
              // The bot is seeing the line at the left side and is therefore heading too far to the right.
              // So the bot should turn left.
              Serial.println("GOTO LEFT");
              out_at_left = false; // Keep track of where the line was last seen.
              // Decrease the speed of the left motor to go left.
              motorLine_r = motorLine_r+LINE_FACTOR*ratio;
            }
            // Decrease the speed a little so the bot doesn't miss the corner.
            motorLine_l = motorLine_l*TURN_SPEED;
            motorLine_r = motorLine_r*TURN_SPEED;
          }
        }else{
          // The bot is heading in the correct direction! No adjustments have to be made.
          off_track = 0;
          // Just light up the green light to show everything is okay :)
          analogWrite(ID_LED_BLUE, 0);
          analogWrite(ID_LED_GREEN, 255);
        }
}


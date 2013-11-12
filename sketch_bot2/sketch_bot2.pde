/* 2
 * This file incorporates all the bits of code into one functional program.
 * Currently implemented:
 * - Hearing the clap (still needs treshold values)
 * - Avoiding collision
 *
 * To implement:
 * - Following the line
 * - Setting speed
 */

#define	ID_ADC_SOUND     0
#define ID_ADC_LINE      2
#define ID_SPEED_R       5
#define ID_SPEED_L       6
#define ID_DIRECTION_R   7
#define ID_DIRECTION_L   8
#define	ID_LEFTRIGHT	12
#define	ID_FRONTREAR	13
#define	ID_ADC_DISTANCE  3

boolean move;

void setup(){
  move = false;
  
  Serial.begin(57600); //For debugging purposes. Ignore if not needed.
  
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
  int sound_left;
  int dist_front;
  int clap_upper = 400;
  int clap_lower = 150;
  int dist = 60;
  
  hearClap();
  if(move){
    keepDistance();
  }
}

void keepDistance(){
  /* Select ADC_DISTANCE FRONT */
  digitalWrite(ID_FRONTREAR, HIGH);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(1);
  dist_front = analogRead(ID_ADC_DISTANCE);

  if(dist_front <= dist){
    //pas snelheid aan
  }
}

void hearClap(){
  /* Select ADC_SOUND LEFT */
  digitalWrite(ID_FRONTREAR, LOW);
  digitalWrite(ID_LEFTRIGHT, LOW);
  delay(1);
  sound_left = analogRead(ID_ADC_SOUND);

  if(sound_left <= clap_lower || sound_left >= clap_upper){
    move = true;
  }
}

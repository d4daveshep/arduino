// Tone test

//static const int buzzerPin = 9;

void setup() {
//  pinMode(9, OUTPUT);
}

void loop() {

//  tone( 9, 500, 250 );
//  noTone(buzzerPin);

  delay(5000);
  
}

void beep(unsigned char delayms){
  analogWrite(9, 20);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(9, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  

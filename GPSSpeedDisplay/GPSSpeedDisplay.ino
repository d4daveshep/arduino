#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <toneAC.h>

/*
 
 The circuit (modified for GPS Shield pins in use):
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 7
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 5
 * LCD D7 pin to digital pin 4
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 * buzzer to digital pin 9
 
 */

static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 4800;

// The TinyGPS++ object
// see http://arduiniana.org/libraries/tinygpsplus/ for library details
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// set up the buzzer for speed alerts
static const boolean alertsOn = true;
static const int buzzerPin = 9;
static const int highTone = 1200;
static const int lowTone = 700;
static const int speedAlert1 = 3; // km/h
boolean alert1fired = false;

void setup() {
  ss.begin(GPSBaud);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");

  // set up speed alerts
  pinMode(buzzerPin, OUTPUT);


}

void loop() {
  // set the cursor to column 0, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0); // first line

  // print date
  lcd.print(gps.date.day());
  lcd.print(".");
  lcd.print(gps.date.month());
  lcd.print(".");
  lcd.print(gps.date.year());
  lcd.print(" ");

  // print time
  uint8_t hr = gps.time.hour();
  lcd.print( hr<10 ? "0" : "");
  lcd.print(hr);
  lcd.print(":");

  uint8_t mn = gps.time.minute();
  lcd.print( mn<10 ? "0" : "" );
  lcd.print(mn);
  lcd.print("  ");

  lcd.setCursor(0, 1); // second line

  // print speed
  float spd = gps.speed.kmph();
  if ( spd < 10.0 ) {
    spd = round(spd*10.0)/10.0;
    lcd.print(spd,1);
  } 
  else {
    spd = round(spd);
    lcd.print(spd,0);
  }
  lcd.print(" km/h ");

  // sound speed alert
  if ( alertsOn ) {
    if ( spd > speedAlert1 && !alert1fired ) {
      // overspeed
//      noToneAC();
      toneAC(highTone,500,5);
//      beep(lowTone,250);
//      beep(highTone,250);
      alert1fired = true;
    } 
    else if ( spd < speedAlert1 && alert1fired ) {
      // underspeedspeed
//      noToneAC();
      toneAC(lowTone,500,5);
//      beep(highTone,250);
//      beep(lowTone,250);
      alert1fired = false;
    }

  }

  // print bearing
  //lcd.print("bng ");
  float bng = round(gps.course.deg());
  lcd.print( bng < 100 ? "0" : "" );
  lcd.print( bng < 10 ? "0" : "" );
  lcd.print(bng,0);
  lcd.write(0b11011111); // degree symbol
  lcd.print("  ");

  smartDelay(500);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    lcd.print("No GPS data");

}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } 
  while (millis() - start < ms);
}

/*
static void beep(unsigned int hz, unsigned int ms) {
  tone(buzzerPin, hz );
  smartDelay(ms);
  noTone(buzzerPin);
}
*/



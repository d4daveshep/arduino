#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

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

void setup() {
  ss.begin(GPSBaud);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");


}

void loop() {
  // set the cursor to column 0, line 0
  // (note: line 1 is the second row, since counting begins with 0):

  // print lat and long on 1 line each
  lcd.setCursor(0, 0); // first line

  // latitude
  uint16_t latDeg = gps.location.rawLat().deg;
  lcd.print(gps.location.rawLat().negative ? "S" : "N");
  lcd.print(latDeg<100 ? " " : "");
  lcd.print(latDeg<10 ? " " : "");
  lcd.print(latDeg);
  lcd.write(0b11011111);
  lcd.print(" ");
  double latMins = gps.location.rawLat().billionths / 1000000000.0 * 60.0;
  lcd.print(latMins<10 ? " " : "");
  lcd.print(latMins,3);
  lcd.print("'");

  lcd.setCursor(0, 1);

  // longitude
  uint16_t lngDeg = gps.location.rawLng().deg;
  lcd.print(gps.location.rawLng().negative ? "W" : "E");
  lcd.print(lngDeg<100 ? " " : "");
  lcd.print(lngDeg<10 ? " " : "");
  lcd.print(lngDeg);
  lcd.write(0b11011111);
  lcd.print(" ");
  double lngMins = gps.location.rawLng().billionths / 1000000000.0 * 60.0;
  lcd.print(lngMins<10 ? " " : "");
  lcd.print(lngMins,3);
  lcd.print("'");


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



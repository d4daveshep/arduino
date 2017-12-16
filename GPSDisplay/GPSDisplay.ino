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
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);

  // print lat and long
  //lcd.print(gps.location.lat());
  //lcd.print(",");
  //lcd.print(gps.location.lng());

  // print date
  lcd.print(gps.date.day());
  lcd.print(".");
  lcd.print(gps.date.month());
  lcd.print(".");
  lcd.print(gps.date.year());
  lcd.print(" ");

  // print time
  lcd.print(gps.time.hour());
  lcd.print(":");
  lcd.print(gps.time.minute());
  //lcd.print(":");
  //lcd.print(gps.time.second());
  lcd.print(" ");
  
  lcd.setCursor(0, 1);
  
  // print bearing
  lcd.print(gps.course.deg());
  lcd.print(" ");
  lcd.print(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "***" );
  lcd.print(" ");
  
  // print speed
  lcd.print(gps.speed.kmph());
  lcd.print(" ");
  
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
  } while (millis() - start < ms);
}


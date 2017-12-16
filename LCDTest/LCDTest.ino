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



// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

// using the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
const int LIGHT_PIN = 10; // pin 10 controls the backlight


void setup() {

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
  
  
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);

  // print date
  lcd.print("hello");
  
  lcd.setCursor(0, 1);
  
  // print bearing
  lcd.print("david");

}



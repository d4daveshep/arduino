/*************************************************************************************
 * 
 * Mark Bramwell, July 2010
 * 
 * This program will test the LCD panel and the buttons.When you push the button on the shield，
 * the screen will show the corresponding one.
 * 
 * Connection: Plug the LCD Keypad to the UNO(or other controllers)
 * 
 **************************************************************************************/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
const int lightPin = 10; // pin 10 controls the backlight

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons(){               // read the buttons
  adc_key_in = analogRead(0);       // read the value from the sensor 

  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  // We make this the 1st option for speed reasons since it will be the most likely result

  if (adc_key_in > 1000) return btnNONE; 

  // For V1.1 us this threshold
  /*
    if (adc_key_in < 50)   return btnRIGHT;  
   if (adc_key_in < 250)  return btnUP; 
   if (adc_key_in < 450)  return btnDOWN; 
   if (adc_key_in < 650)  return btnLEFT; 
   if (adc_key_in < 850)  return btnSELECT;  
   */

  // For V1.0 comment the other threshold and use the one below: 
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 195)  return btnUP; 
  if (adc_key_in < 380)  return btnDOWN; 
  if (adc_key_in < 555)  return btnLEFT; 
  if (adc_key_in < 790)  return btnSELECT;   

  return btnNONE;                // when all others fail, return this.
}

int light = 100; // set light at 100%

void setup(){
  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  lcd.print("Backlight UP/DN");  // print a simple message on the LCD
  analogWrite(lightPin,light*25);

}

void loop(){
  lcd_key = read_LCD_buttons();   // read the buttons

    switch (lcd_key){               // depending on which button was pushed, we perform an action

  case btnRIGHT:
    {             //  push button "RIGHT" and show the word on the screen
      break;
    }
  case btnLEFT:
    {
      break;
    }    
  case btnUP:
    {
      // increase light by 10%
      if (light < 100 ) {      
        light = light + 10;
      }
      break;
    }
  case btnDOWN:
    {
      // decrease light by 10%
      if (light > 0) {
        light = light - 10;
      }
      break;
    }
  case btnSELECT:
    {
      break;
    }
  case btnNONE:
    {
      break;
    }
  }
  lcd.setCursor(0,1);             // move to the begining of the second line
  lcd.print(light);
  lcd.print("%  ");
  analogWrite(lightPin,light*25); // update the light amount
  delay(100);
  
}


/*************************************************************************************
 * 
 * Based on the LCDButton test sketch but modified to read up to 21V on pin A1
 * v1 just reads voltage and displays on screen
 * v2 turn LCD backlight on/off 
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


// Analog volt read pin
const int voltPin = 1;

const float calibration = 0.993; // calibration factor for correcting voltage around the 12-13V range

//Variables for voltage divider
float denominator;
// actual resistor values
int resistor1 = 14890; // nominal 15000
int resistor2 = 4610; // nominal 4700; 

boolean lightOn = true;

void setup(){
  Serial.begin(9600);
  analogReference(DEFAULT);

  //  Convert resistor values to division value
  //  R2 / (R1 + R2)
  denominator = (float)resistor2 / (resistor1 + resistor2);

  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  lcd.print("Voltmeter v2.0");   // print title message


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
      lightOn = true;  // turn backlight on
      break;
    }
  case btnDOWN:
    {
      lightOn = false;  // turn backlight off
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
  if ( lightOn ) {
    analogWrite(lightPin,255); // turn light on
  } else {
    analogWrite(lightPin,0); // turn light off
  }


  lcd.setCursor(0,1);             // move to the begining of the second line

  float voltage;
  //Obtain RAW voltage data
  voltage = analogRead(voltPin);
  Serial.print("pin value=");
  Serial.print(voltage);

  // Convert to actual voltage (0 - 5 Vdc)
  voltage = (voltage / 1024.0) * 5.0; // scale to 0 - 5 V range
  //  voltage = (voltage / 1024) * 4.2;  // use 4.2 as it's 21Vmax / 5

  // print pin voltage 
  lcd.print(voltage);
  lcd.print(" ");
  
  Serial.print(" pin voltage=");
  Serial.print(voltage);


  //Convert to voltage before divider
  //  Divide by divider = multiply
  //  Divide by 1/5 = multiply by 5
  voltage = voltage / denominator * calibration;
  
  Serial.print(" divided voltage=");
  Serial.println(voltage);


  // print divided voltage
  lcd.print(voltage);
  lcd.print(" V ");

  //Delay to make serial out readable
  delay(500);



}




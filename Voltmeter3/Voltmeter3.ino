/*************************************************************************************
 * 
 * Based on the LCDButton test sketch but modified to read up to 21V on pin A1
 * v1 just reads voltage and displays on screen
 * v2 turn LCD backlight on/off
 * v3 add averaging for volt readings
 * 
 **************************************************************************************/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
const int LIGHTPIN = 10; // pin 10 controls the backlight

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
boolean lightOn = true;  // keep track of whether the LCD light is on or off

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


const int VOLTPIN = 1;  // Analog volt read pin
const float VOLT_CALIBRATION = 0.993; // calibration factor for correcting voltage around the 12-13V range
const int NUM_VOLT_READINGS = 10; // number of volt readings to average over

int voltReadings[NUM_VOLT_READINGS]; // array to hold volt readings
int voltIndex = 0; // index of the current reading
int voltTotal = 0; // total of all the volt readings
float voltAverage = 0.0; // average volt readings

// Variables for voltage divider
float denominator;
// actual resistor values for voltage divider, measured by multimeter
int resistor1 = 14890; // nominal 15000
int resistor2 = 4610; // nominal 4700; 

void setup(){
  Serial.begin(9600);
  analogReference(DEFAULT);

  //  Convert resistor values to division value
  //  R2 / (R1 + R2)
  denominator = (float)resistor2 / (resistor1 + resistor2);

  // initialise the volt readings to 0
  for ( int thisReading = 0; thisReading < NUM_VOLT_READINGS; thisReading++ ) {
    voltReadings[thisReading] = 0;
  }

  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  lcd.print("Voltmeter v3.0");   // print title message


}

void loop(){
  lcd_key = read_LCD_buttons();   // read the buttons

    switch (lcd_key) {               // depending on which button was pushed, we perform an action

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
    analogWrite(LIGHTPIN,255); // turn light on
  } 
  else {
    analogWrite(LIGHTPIN,0); // turn light off
  }

  lcd.setCursor(0,1);             // move to the begining of the second line

  // do the voltage readings and average calculation

  voltTotal -= voltReadings[voltIndex]; // subtract the last volt reading
  voltReadings[voltIndex] = analogRead(VOLTPIN); // read the volt pin into index location
  voltTotal += voltReadings[voltIndex]; // add the new volt reading to the total
  voltIndex++; // increment the volt index
  // reset the volt index if at the end of the array
  if ( voltIndex >= NUM_VOLT_READINGS ) {
    voltIndex = 0;
  }

  voltAverage = voltTotal / NUM_VOLT_READINGS; // calculate the average

  Serial.print("pin value=");
  Serial.print(voltAverage);

  // Convert to actual voltage (0 - 5 Vdc)
  voltAverage = (voltAverage / 1024.0) * 5.0; // scale to 0 - 5 V range
  //  voltage = (voltage / 1024) * 4.2;  // use 4.2 as it's 21Vmax / 5

  // print pin voltage 
  lcd.print(voltAverage);
  lcd.print(" ");

  Serial.print(" pin voltage=");
  Serial.print(voltAverage);


  //Convert to voltage before divider
  //  Divide by divider = multiply
  //  Divide by 1/5 = multiply by 5
  voltAverage = voltAverage / denominator * VOLT_CALIBRATION;

  Serial.print(" divided voltage=");
  Serial.println(voltAverage);


  // print divided voltage
  lcd.print(voltAverage);
  lcd.print(" V ");

  //Delay to make serial out readable
  delay(10);

}






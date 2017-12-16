/*************************************************************************************
 * 
 * Based on the LCDButton test sketch but modified to read up to 21V on pin A1
 * v1 based on Voltmeter v4.1
 *    add voltage calibration against internal Vcc
 *    add code for current ACS715 sensor and calibrate
 * 
 **************************************************************************************/

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
const int LIGHT_PIN = 10; // pin 10 controls the backlight

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


const int VOLT_PIN = 1;  // Analog volt read pin
//const float VOLT_CALIBRATION = 0.993; // calibration factor for correcting voltage around the 12-13V range
const float VOLT_CALIBRATION = 1.0; // calibration factor for correcting voltage around the 12-13V range
const int NUM_VOLT_READINGS = 10; // number of volt readings to average over

const int AMP_PIN = 2;  // Analog amp read pin
const int NUM_AMP_READINGS = 10; // number of amp readings to average over



int voltReadings[NUM_VOLT_READINGS]; // array to hold volt readings
int voltIndex = 0; // index of the current reading
int voltTotal = 0; // total of all the volt readings
float voltAverage = 0.0; // average volt reading
double Vcc; // internal Vcc

int ampReadings[NUM_AMP_READINGS]; // array to hold amp readings
int ampIndex = 0; // index of the current reading
int ampTotal = 0; // total of all the amp readings
float ampAverage = 0; // average amp reading from the pin
float amps = 0; // calibrated amps value

// Variables for voltage divider
float denominator;
// actual resistor values for voltage divider, measured by multimeter
int resistor1 = 14890; // nominal 15000
int resistor2 = 4610; // nominal 4700; 

// function to read internall Vcc
long readVcc() {
  long result; 
  ADMUX = _BV(REFS0)  | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC); //convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH <<8;
//  result = 1125300L / result;  // assumes exactly 1.1V ref
  result = 1141609L / result; // acutal measurement of Vcc was 4.9 vs 4.83 from 1125300 value
  return result;
}

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

  // initialise the amp readings to 0
  for ( int thisReading = 0; thisReading < NUM_AMP_READINGS; thisReading++ ) {
    ampReadings[thisReading] = 0;
  }

  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0,0);             // set the LCD cursor   position 
  lcd.print("BattMon v1");   // print title message

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
    analogWrite(LIGHT_PIN,255); // turn light on
  } 
  else {
    analogWrite(LIGHT_PIN,0); // turn light off
  }

  lcd.setCursor(0,1);             // move to the begining of the second line

  // do the voltage readings and average calculation
  voltTotal -= voltReadings[voltIndex]; // subtract the last volt reading
  voltReadings[voltIndex] = analogRead(VOLT_PIN); // read the volt pin into index location
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
  Vcc = readVcc() / 1000.0; // read internal Vcc
  voltAverage = (voltAverage / 1024.0) * Vcc; // scale to 0 - 5 V range
//  voltAverage = (voltAverage / 1024.0) * 5.0; // scale to 0 - 5 V range

  // print pin voltage 
//  lcd.print(voltAverage);
//  lcd.print(" ");

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
  lcd.print("V ");


  // do the amp readings and average calculation
  ampTotal -= ampReadings[ampIndex]; // subtract the last volt reading
  ampReadings[ampIndex] = analogRead(AMP_PIN); // read the volt pin into index location
  ampTotal += ampReadings[ampIndex]; // add the new volt reading to the total
  ampIndex++; // increment the volt index
  // reset the volt index if at the end of the array
  if ( ampIndex >= NUM_AMP_READINGS ) {
    ampIndex = 0;
  }

  ampAverage = ampTotal / NUM_AMP_READINGS; // calculate the average
  
  // calculate the amp value from the sensor as follows:
  /* The ACS715 sensor outputs about 100 at rest. Analog read produces a value of 0-1023, equating to 0v to 5v. 
     ((long)sensorValue * 5000 / 1024) is the voltage on the sensor's output in millivolts. 
     There's a 500mv offset to subtract. The unit produces 133mv per amp of current.*/ 
//  amps = (((float)ampAverage * 5000.0 / 1024.0) - 500.0 ) / 133.0; // * 1000.0;
  amps = (((float)ampAverage * Vcc * 1000.0 / 1023.0) - 478.0 ) / 133.0;

  // print amps value
  lcd.print(amps);
  lcd.print("A ");


  //Delay to make serial out readable
  delay(100);

}






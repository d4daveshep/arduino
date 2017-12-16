
/*************************************************************************************
 * 
 * Based on the LCDButton test sketch but modified to read up to 21V on pin A1
 * v1 based on Voltmeter v4.1
 *    add voltage calibration against internal Vcc
 *    add code for current ACS715 sensor and calibrate
 * v2 calculate power
 * v3 calculate amp hours
 * v4 improve amp hours calculation
 * v4.1 adjusted volt calibration for 12+V power via Vin
 * v5 changed to ACS714 chip and sensor code
 * 
 **************************************************************************************/

#include <LiquidCrystal.h>
#include <Time.h>

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

const int SAMPLE_DELAY = 10; // millisec delay between samples

const int VOLT_PIN = 1;  // Analog volt read pin
//const float VOLT_CALIBRATION = 0.993; // calibration factor for correcting voltage around the 12-13V range
const float VOLT_CALIBRATION = 1.016; // calibration factor for correcting voltage around the 12-13V range
const int NUM_VOLT_READINGS = 10; // number of volt readings to average over

const int AMP_PIN = 2;  // Analog amp read pin
const int NUM_AMP_READINGS = 10; // number of amp readings to average over



int voltReadings[NUM_VOLT_READINGS]; // array to hold volt readings
int voltIndex = 0; // index of the current reading
int voltTotal = 0; // total of all the volt readings
double voltAverage = 0.0; // average volt reading
double Vcc; // internal Vcc

int ampReadings[NUM_AMP_READINGS]; // array to hold amp readings
int ampIndex = 0; // index of the current reading
int ampTotal = 0; // total of all the amp readings
double ampAverage = 0; // average amp reading from the pin
double amps = 0; // calibrated amps value

double watts = 0;

double hoursElapsed = 0.0; // number of hours since startup
double ampHours = 0.0; // this is the total amp hours flowed since startup

int timeSlice = 0; // duration of the current time slice in milli sec
unsigned long oldMillis = 0;
unsigned long nowMillis = 0;
double ampMsSlice = 0.0;

// Variables for voltage divider
double denominator;
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
//  lcd.print("BattMon v3");   // print title message

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

  lcd.setCursor(0,0);             // move to the begining of the first line

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

//  Serial.print("pin value=");
  Serial.print(voltAverage);
  Serial.print(" " );

  // Convert to actual voltage (0 - 5 Vdc)
  Vcc = readVcc() / 1000.0; // read internal Vcc
  voltAverage = (voltAverage / 1024.0) * Vcc; // scale to 0 - 5 V range
//  voltAverage = (voltAverage / 1024.0) * 5.0; // scale to 0 - 5 V range

  // print pin voltage 
//  lcd.print(voltAverage);
//  lcd.print(" ");

//  Serial.print(" pin voltage=");
  Serial.print(voltAverage);
  Serial.print("V ");


  //Convert to voltage before divider
  //  Divide by divider = multiply
  //  Divide by 1/5 = multiply by 5
  voltAverage = voltAverage / denominator * VOLT_CALIBRATION;

//  Serial.print(" divided voltage=");
  Serial.print(voltAverage);
  Serial.print("V ");


  // print divided voltage
  lcd.print(voltAverage);
  lcd.print("V ");


  // do the amp readings and average calculation
  ampTotal -= ampReadings[ampIndex]; // subtract the last volt reading
  ampReadings[ampIndex] = analogRead(AMP_PIN); // read the amp pin into index location
  ampTotal += ampReadings[ampIndex]; // add the new volt reading to the total
  ampIndex++; // increment the volt index
  // reset the volt index if at the end of the array
  if ( ampIndex >= NUM_AMP_READINGS ) {
    ampIndex = 0;
  }

  ampAverage = ampTotal / NUM_AMP_READINGS; // calculate the average
  
  amps = currentSensor(ampAverage);
/*
  if(amps<0) {
    amps=0.0;
  }
*/

  Serial.print( amps );
  Serial.print( "A ");
  
  watts = amps * voltAverage;

  // print amps value
  lcd.print(amps);
  lcd.print("A ");
  
  lcd.print( watts );
  lcd.print( "W " );

  nowMillis = millis();
  hoursElapsed = nowMillis / 3600000.0;

  lcd.setCursor(0,1);             // move to the begining of the second line
  lcd.print(hour());
  lcd.print(":");
  lcd.print(formatTimeDigits(minute()));
  lcd.print(" ");

  timeSlice = nowMillis - oldMillis;
  oldMillis = nowMillis;
  Serial.print(timeSlice);
  Serial.print("ms ");

//  secsElapsed = ; // secs elapsed since startup - will overflow to zero in ~50 days
  Serial.print(nowMillis / 1000.0);
  Serial.print("s ");

  Serial.print(hour());
  Serial.print(":");
  Serial.print(formatTimeDigits(minute()));
  Serial.print(" ");  

  ampMsSlice = amps * timeSlice;
  Serial.print(ampMsSlice);
  Serial.print("Ams ");
  
  ampHours += ampMsSlice / 1000.0 / 3600.0;

  Serial.print(ampHours);
  Serial.print("Ah ");

  lcd.print( ampHours );
  lcd.print( "Ah " );

  Serial.println();


  //Delay to make LCD readable
  delay(SAMPLE_DELAY);

}

String formatTimeDigits(int num) {
  String doubleDigits = "";
  if(num<10) {
    doubleDigits = "0";
  }
  doubleDigits = doubleDigits + num;
  return doubleDigits;
}
  
// Calculate current with ACS714 from http://playground.arduino.cc/Main/CurrentSensing
double currentSensor(int RawADC) {

  int    Sensitivity    = 66; // mV/A

  long   InternalVcc    = readVcc();
  double ZeroCurrentVcc = InternalVcc / 2;
  double SensedVoltage  = (RawADC * InternalVcc) / 1024;
  double Difference     = SensedVoltage - ZeroCurrentVcc;
  double SensedCurrent  = Difference / Sensitivity;
/*
  Serial.print("ADC: ");
  Serial.print(RawADC);
  Serial.print("/1024");

  Serial.print(", Sensed Voltage: ");
  printDouble(SensedVoltage, 1);
  Serial.print("mV");

  Serial.print(", 0A at: ");
  printDouble(ZeroCurrentVcc, 1);
  Serial.print("mV");
*/
  return SensedCurrent;                                        // Return the Current

} 

void printDouble(double val, byte precision) {

  Serial.print (int(val));                                     // Print int part

  if( precision > 0) {                                         // Print decimal part
    Serial.print(".");
    unsigned long frac, mult = 1;
    byte padding = precision -1;
    while(precision--) mult *=10;
    if(val >= 0) frac = (val - int(val)) * mult; else frac = (int(val) - val) * mult;
    unsigned long frac1 = frac;
    while(frac1 /= 10) padding--;
    while(padding--) Serial.print("0");
    Serial.print(frac,DEC) ;
  }

} 




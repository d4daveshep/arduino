/*
 *  Fermentation Controller
 *
 *  v2 Changed over to DS18B20 temp sensor and added 60 sec average temp reading.
 *  v3 Corrected min and max by setting average temp to first temp reading.
 *     Improved changed decision logic so that target temp will always be reached before changing action.
 */

#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// define the temp data variables
const int NUM_READINGS = 60;
double tempReadings[NUM_READINGS]; // array to hold temp readings
int tempIndex = 0; // index of the current reading
double tempTotal = 0; // total of all the temp readings
double tempAverage = 0.0; // average temp reading



// initialize the library with the numbers of the interface pins
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

#define HEAT_RELAY 11 //  Heating relay pin
#define COOL_RELAY 12 // Cooling relay pin

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

const byte degreeSymbol = B11011111;

const float TARGET_TEMP = 23.0; // the target temperature of the fermentation chamber
const float TEMP_DIFF = 0.3; // the tolerance we allow before taking action

//float currentTemp = 0.0;
float minTemp = 1000.0;
float maxTemp = -1000.0;

const int REST = 0;
const int HEAT = 1;
const int COOL = 2;
int currentAction = REST; // the current thing we're doing (e.g. REST, HEAT or COOL)



void setup(void) {
  Serial.begin(9600);

  // set up the relay pins
  pinMode(HEAT_RELAY,OUTPUT);
  pinMode(COOL_RELAY,OUTPUT);

  // set up the LCD as 16 x 2
  lcd.begin(16,2);

  // set up the temp sensors
  sensors.begin();

  sensors.requestTemperatures();  
  double firstReading = sensors.getTempCByIndex(0); // read the temp into index location

  Serial.print("first reading=");
  Serial.println(firstReading);

  // initialise the temp readings to 0
  for ( int thisReading = 0; thisReading < NUM_READINGS; thisReading++ ) {
    tempReadings[thisReading] = firstReading;
    //tempReadings[thisReading] = 0;
  }
  tempAverage = firstReading;
  tempTotal = tempAverage * NUM_READINGS;

  delay(1000);
}

void loop(void) {
  char buf[6];

  sensors.requestTemperatures();  

  // do the temp readings and average calculation
  tempTotal -= tempReadings[tempIndex]; // subtract the last temp reading
  tempReadings[tempIndex] = sensors.getTempCByIndex(0); // read the temp into index location

  Serial.print("target=");
  Serial.print(TARGET_TEMP);
  Serial.print("+/-");
  Serial.print(TEMP_DIFF);
  Serial.print(" temp=");
  Serial.print(tempReadings[tempIndex]);

  tempTotal += tempReadings[tempIndex]; // add the new temp reading to the total
  tempIndex++; // increment the temp index
  // reset the temp index if at the end of the array
  if ( tempIndex >= NUM_READINGS ) {
    tempIndex = 0;
  }

  tempAverage = tempTotal / NUM_READINGS; // calculate the average
  Serial.print(" avg=" );
  Serial.print(tempAverage);


  // update the max and min values TODO after 60 readings
  if (tempAverage > maxTemp) {
    maxTemp = tempAverage;
  }
  if (tempAverage < minTemp) {
    minTemp = tempAverage;
  }

  // what are we doing and do we need to change
  switch ( currentAction ) {
  case REST:
    // are we within tolerance
    if ( tempAverage < (TARGET_TEMP-TEMP_DIFF) ) {
      // we are too cold so start heating
      currentAction = HEAT;
    } 
    else if ( tempAverage > (TARGET_TEMP+TEMP_DIFF) ) {
      // we are too hot so start cooling
      currentAction = COOL;
    } 
    else {
      // we are within tolerance so keep resting
      currentAction = REST;
    }
    break;

  case HEAT:
    // have we reached or exceeded our target yet, but we don't want to overshoot
    if ( tempAverage >= ( TARGET_TEMP - TEMP_DIFF/2 )) {
      // yes so rest
      currentAction = REST;
    }
    break;

  case COOL:
    // have we reached or exceeded our target yet, but we don't wnat to overshoot
    if ( tempAverage <= ( TARGET_TEMP + TEMP_DIFF/2 ) ) {
      // yes so rest
      currentAction = REST;
    }
    break;







  }

  // do the action
  switch ( currentAction) {

  case REST:
    digitalWrite(HEAT_RELAY,LOW); // turn the Heat off
    digitalWrite(COOL_RELAY,LOW); // turn the Cool off
    break;

  case HEAT:  
    digitalWrite(HEAT_RELAY,HIGH); // turn the Heat on
    digitalWrite(COOL_RELAY,LOW); // turn the Cool off
    break;

  case COOL:
    digitalWrite(HEAT_RELAY,LOW); // turn the Heat off
    digitalWrite(COOL_RELAY,HIGH); // turn the Cool on
    break;

    //  default:
  }

  //  fahrenheit = celsius * 1.8 + 32.0;

  Serial.print(" min=");
  Serial.print(minTemp);
  Serial.print(" max=");
  Serial.print(maxTemp);
  Serial.print(" ");

  // print line 1  
  lcd.setCursor(0,0);
  lcd.print("aim");
  lcd.print( dtostrf(TARGET_TEMP,4,1,buf) );
  lcd.print(" ");

  lcd.print("now");
  lcd.print( dtostrf(tempAverage,4,1,buf) );
  lcd.print(" ");

  //print line 2
  lcd.setCursor(0,1);

  switch ( currentAction) {

  case REST:
    lcd.print("Rest");
    Serial.print("Rest");
    break;
  case HEAT:
    lcd.print("Heat");
    Serial.print("Heat");
    break;
  case COOL:
    lcd.print("Cool");
    Serial.print("Cool");
    break;
  default:
    lcd.print("ERROR");
    Serial.print("ERROR");

  }


  //  lcd.print("Heat");
  lcd.print(" ");
  //  lcd.print("Lo ");
  lcd.print( dtostrf(minTemp,4,1,buf) );
  lcd.print("-");
  lcd.print( dtostrf(maxTemp,4,1,buf) );

  Serial.println();


  delay(1000);

}









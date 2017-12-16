#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 3     // what pin we've connected the temp sensor to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);


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

const float TARGET_TEMP = 18.0; // the target temperature of the fermentation chamber
const float TEMP_DIFF = 0.5; // the tolerance we allow before taking action

float currentTemp = 0.0;
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

  dht.begin();

}

void loop(void) {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float currentTemp = dht.readTemperature();
  char buf[6];

  // update the max and min values
  if (currentTemp > maxTemp) {
    maxTemp = currentTemp;
  }
  if (currentTemp < minTemp) {
    minTemp = currentTemp;
  }

  // decide what to do
  if ( currentTemp < ( TARGET_TEMP - TEMP_DIFF ) ) {
    currentAction = HEAT;
  } 
  else if ( currentTemp > ( TARGET_TEMP + TEMP_DIFF ) ) {
    currentAction = COOL;
  } 
  else {
    currentAction = REST;
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
  /*
  Serial.print("  Temp = ");
   Serial.print(currentTemp);
   Serial.print("*C, min=");
   Serial.print(minimum);
   Serial.print("*C, max=");
   Serial.print(maximum);
   Serial.print("*C");
   //  Serial.print(fahrenheit);
   //  Serial.println(" Fahrenheit");
   Serial.println();
   */

  // print line 1  
  lcd.setCursor(0,0);
  lcd.print("aim");
  lcd.print( dtostrf(TARGET_TEMP,4,1,buf) );
  lcd.print(" ");

  lcd.print("now");
  lcd.print( dtostrf(currentTemp,4,1,buf) );

  //print line 2
  lcd.setCursor(0,1);

  switch ( currentAction) {

  case REST:
    lcd.print("Rest");
    break;
  case HEAT:
    lcd.print("Heat");
    break;
  case COOL:
    lcd.print("Cool");
    break;
  default:
    lcd.print("ERROR");

  }


  //  lcd.print("Heat");
  lcd.print(" ");
  //  lcd.print("Lo ");
  lcd.print( dtostrf(minTemp,4,1,buf) );
  lcd.print("-");
  lcd.print( dtostrf(maxTemp,4,1,buf) );
  
  delay(500);
  
}






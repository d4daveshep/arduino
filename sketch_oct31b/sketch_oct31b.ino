/*
Code It, Build It, Done!

Arduino, Android and other personal projects
Sunday, June 14, 2015
Arduino Power Logger
Measuring the voltage and current of a device to see the power output can be useful for DC or battery powered systems. This can be done with a digital voltmeter, but I wanted to log it (in a text file) also so I could graph it and run analysis on it. I found a similar Arduino power monitor project online, but I didn't need the screen display. So I added the Logger function with the Ethernet Shield Arduino. Here are the steps I took to make my own Power Logger.


List of Parts

    Arduino (I recommend the Arduino starter kit if you don't have an Arduino yet)
    Ethernet Shield Arduino (with SD card ready for logging data)
    Power Supply to power device
    Breadboard 
    Resistors (for voltage Divider) - Actual values depend on voltages used. The example here scales 12 volts to 5 volts for the Arduino to read easily. Read more about voltage dividers to determine your resistor values.
    Jumper wires
    ACS715 Hall Effect Current Sensor
    A device you want to measure the power consumption of and you can power it up with a power supply


I should note that the ACS715 current sensor requires you to power the device through this sensor, so you will need the leads from your device be connected to one end of this device and the power supply to run your device connected to the other end.

Build It
Setup the components with the Arduino as shown in the Fritzing Diagram below.

Note that the ACS715 hall sensor device has 5 connections. 3 plug into the breadboard: Gnd, Vcc, and Out. These plug into the breadboard and the Arduino respectively. The Fritzing Diagram does not show this 100% accurately since the Current sensor is not the same in the Fritzing program, but it should be easy to determine. The other two ports on the sensor run the current through your device when powered up.
Code It
The code can be broken up into 4 sections. Setup, Measuring voltage (with the voltage divider), measuring current with the ACS715 sensor, and logging the output to the SD card. The latest code for the Arduino Power Logger can be found here on Github.
*/

#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Time.h>

File myFile;

long readVcc() {
  long result; 
  ADMUX = _BV(REFS0)  | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC); //convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH <<8;
  result = 1125300L / result;
  return result;
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.print(" ");
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
   
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

int batMonPin = A0;    // input pin for the voltage divider
int batVal = 0;       // variable for the A/D value
float pinVoltage = 0; // variable to hold the calculated voltage
float batteryVoltage = 0;

unsigned int ADCValue;
double Voltage;
double Vcc;

int analogInPin = A1;  // Analog input pin that the carrier board OUT is connected to
int sensorValue = 0;        // value read from the carrier board
int outputValue = 0;        // output in milliamps
unsigned long msec = 0;
float time = 0.0;
int sample = 0;
float totalCharge = 0.0;
float averageAmps = 0.0;
float ampSeconds = 0.0;
float ampHours = 0.0;
float wattHours = 0.0;
float amps = 0.0;

int R1 = 9890; // Resistance of R1 in ohms
int R2 = 4620; // Resistance of R2 in ohms
float ratio = 0;  // Calculated from R1 / R2

void setup() {
  Serial.begin(9600);
  setTime(0,0,0,11,5,2015);
  Serial.print("Initializing SD card....");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
   if(!SD.begin(4)){
    Serial.println("initialization failed!");
    return;
   }
    Serial.println("initialization done.");
    myFile = SD.open("PowerLog.txt", FILE_WRITE);
    
    if(myFile){
  Serial.print("Writing to testLog.txt...");
  myFile.println("testing data log");
  //Close the file writer
    myFile.close();
}else{
  Serial.println("error opening text file");
}
}

void loop() {
digitalClockDisplay();
int sampleBVal = 0;
int avgBVal = 0; 
int sampleAmpVal = 0;
int avgSAV = 0;

for (int x = 0; x < 10; x++){ // run through loop 10x

  // read the analog in value:
  
 // batVal = analogRead(batMonPin);    // read the voltage on the divider
 // sampleBVal = sampleBVal + batVal; // add samples together
  sensorValue = analogRead(analogInPin);  
  sampleAmpVal = sampleAmpVal + sensorValue; // add samples together
 
  delay (10); // let ADC settle before next sample
}

//avgBVal = sampleBVal / 10; //divide by 10 (number of samples) to get a steady reading
avgSAV = sampleAmpVal / 10;



  
/* sensor outputs about 100 at rest.
Analog read produces a value of 0-1023, equating to 0v to 5v.
"((long)sensorValue * 5000 / 1024)" is the voltage on the sensor's output in millivolts.
There's a 500mv offset to subtract.
The unit produces 133mv per amp of current, so
divide by 0.133 to convert mv to ma
         
*/
batVal = analogRead(batMonPin);
Vcc = readVcc()/1000.0;
pinVoltage = (batVal/1023.0)*Vcc;
ratio = ( ((float)R1+(float)R2)/(float)R2)/.96962; //.96962 is calculate fudge factor
batteryVoltage = pinVoltage * ratio; 


//sensorValue = analogRead(analogInPin);
//sampleAmpVal = sampleAmpVal + sensorValue;
outputValue = (((long)avgSAV * 5000 / 1024) - 500 ) * 1000 / 133; 
 
amps = (float) outputValue / 1000;
float watts = amps * batteryVoltage;

  
  //Serial.print(dateString);
  Serial.print("\t ");
  Serial.print("Volts = " );
  Serial.print(batteryVoltage);
  Serial.print("\t Current (amps) = ");
  Serial.print(amps);
  Serial.print("\t Power (Watts) = ");  
  Serial.print(watts); 
  
  sample = sample + 1;
 
  msec = millis();
 
  time = (float) msec / 1000.0;
 
  totalCharge = totalCharge + amps;
 
  averageAmps = totalCharge / sample;
  ampSeconds = averageAmps*time;
  
  ampHours = ampSeconds/3600;
 
  wattHours = batteryVoltage * ampHours;
  
  Serial.print("\t Time (hours) = ");
  Serial.print(time/3600);
 
  Serial.print("\t Amp Hours (ah) = ");
  Serial.print(ampHours);
  Serial.print("\t Watt Hours (wh) = ");
  Serial.println(wattHours);
  
   //Open the file writer
  myFile = SD.open("PowerLog.txt", FILE_WRITE);  
 if(myFile){
    //myFile.print(dateString);
    myFile.print("Volts = " );                      
    myFile.print(batteryVoltage);
    myFile.print("\t Current (amps) = ");
    myFile.print(amps);
    myFile.print("\t Power (Watts) = "); 
    myFile.print(watts);
    myFile.print("\t Time (hours) = ");
    myFile.print(time/3600);
    myFile.print("\t Amp Hours (ah) = ");
    myFile.print(ampHours);
    myFile.print("\t Watt Hours (wh) = ");
    myFile.print(wattHours);
    myFile.print("\r\n");
    
    //Close the file writer
    myFile.close();
  
 }
  
  delay(1000);
}


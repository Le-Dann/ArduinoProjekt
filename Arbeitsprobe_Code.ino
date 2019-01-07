#include <LiquidCrystal.h> //LCD library
LiquidCrystal lcd(13,12,11,10,9,8); //sets up pins connected to LCD display

#include <elapsedMillis.h> //library introduces elapsedmillis varibale
elapsedMillis tim; // variable stores amount of time elapsed in milliseconds
#define interval 5000//defined length of time sensor is calibrated in milliseconds

const int sensorPin = A0; //const value stores sensor pin being used to check analog readings from tempertaure sensor
float baselineTemp = 20.0; // every 2 degrees above this baseline temp, turns on the next LED, default baseline temperature is set to 20 degrees celsius
float temperature= 0.0; //stores value of the temperature when voltage is converted
int sensorVal = 0; // stores the value read by the temperature sensor
int switchState = 0; //switch used to calibrate room temperature.
int sensorMin = 1023; // minimum sensor value reading set first to 1023
int sensorMax = 0; // maximum sensor value reading set first to 0
int switchpin = 5; //stores value at which input is the switch pin read
int LEDCalibrate = 6; // stores value at which output LED pin used for calibration is at
void setup() {
  Serial.begin(9600); //open a serial port connection between Arduino and computer, so analog values that are input appear on serial monitor. 
                     //9600 represents bits per second transfer rate.
  for(int pinNumber = 2; pinNumber<5; pinNumber++){ //for loop simplifies declaration of pinMode running through pins 2 to 4.
      pinMode(pinNumber,OUTPUT);
      digitalWrite(pinNumber,LOW);
    }
   
    
     pinMode(switchpin,INPUT);   //reads switch used to turn on LED to signal the start of the calibration period.
    pinMode(LEDCalibrate, OUTPUT);
    digitalWrite(LEDCalibrate, LOW);
    tim=0;
    lcd.begin(16,2); //sets up number of rows and columns on LCD
}

void loop() {
 switchState = digitalRead(switchpin);
 if(switchState == LOW){ //as long as switch is not pressed baseline temperature is not changed and program continues to read temperature
  int sensorVal = analogRead(sensorPin); //Analog read will be an integer between 0 and 1023. 
  Serial.print("\nSensor Value: "); 
  Serial.print(sensorVal);
  
  float voltage = (sensorVal/1024.0) * 5.0; //converts analog reading reading to volatage. Analog reading goes up to 1024 and voltage goes up to 5V.
  Serial.print(", Volts: ");
  Serial.print(voltage);
  
  Serial.print(", Degrees C: ");
  float temperature = (voltage - .5) * 100; // datasheet provided for the sensor indicates what temperature is being read based on the voltage. 
                                           //In this case every 10 millivolts of change represents 1 degree Celsius of temp change
  Serial.print(temperature);

  if(temperature < baselineTemp + 1){
          digitalWrite(2,LOW);
          digitalWrite(3,LOW);
          digitalWrite(4,LOW);
    }
    else if (temperature >= baselineTemp+2 && temperature < baselineTemp+4){
          digitalWrite(2,HIGH);
          digitalWrite(3,LOW);
          digitalWrite(4,LOW);
    }else if (temperature >= baselineTemp+4 && temperature < baselineTemp+6){
          digitalWrite(2,HIGH);
          digitalWrite(3,HIGH);
          digitalWrite(4,LOW);
    }else if (temperature >= baselineTemp+6){
          digitalWrite(2,HIGH);
          digitalWrite(3,HIGH);
          digitalWrite(4,HIGH);
    }
        lcd.setCursor(0,1);
        lcd.print("Temp Deg C: ");
        lcd.print(temperature);
        delay(1000); //Analog readings can only be done so fast hence delay.
   }
  else//if(switchState == HIGH)
   { //once switch is pressed temperature is read and baseline temperature is recalibrated. Takes 5 seconds
      tim=0;//set timer to go back to zero
       while (tim < interval) { //as long as time elapsed is less than 5 seconds
     digitalWrite(LEDCalibrate,HIGH); // LED at pin 9 is turned on during calibration
     sensorVal= analogRead(sensorPin);
     // record the maximum sensor value
    if (sensorVal > sensorMax) {
      sensorMax = sensorVal;
    }

    // record the minimum sensor value
    if (sensorVal < sensorMin) {
      sensorMin = sensorVal;
    }
  }
        sensorVal = (sensorMax + sensorMin)/2; //determines the average sensor reading within the 5 second time frame 
        float voltage = (sensorVal/1024.0) * 5.0; 
  
        
        float temperature = (voltage - .5) * 100; 
       
        baselineTemp = temperature;
        Serial.print("\n New base temperature: ");
        Serial.print(baselineTemp);  
        lcd.setCursor(0,1);
        lcd.write("base Temp: ");
        lcd.write(baselineTemp);
        delay(5000);
        digitalWrite(LEDCalibrate,LOW); // LED at pin 9 is turned off once calibration is finished
}
}
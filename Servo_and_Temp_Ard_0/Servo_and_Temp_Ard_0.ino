#include <Servo.h>
#define aref_voltage 3.3
int servoPos = 0;
int tempPin = 0;
float voltage = 0;
float degreesC = 0;

Servo myServo;

int tempReading; // the analog reading from the sensor

void setup(void)
{
  Serial.begin(9600);
  analogReference(EXTERNAL); // If you want to set the aref to something other than 5v
  myServo.attach(9);
}

void loop(void) {

  tempReading = analogRead(tempPin);

  // converting that reading to voltage, which is based off the reference voltage
  float voltage = tempReading * aref_voltage;
   voltage /= 1024.0;
  
  // print out the voltage
  Serial.print("Voltage: ");
  Serial.print(voltage); Serial.print(" volts, ");

  // now print out the temperature
  Serial.print("Temperature: ");
  float temperatureC = (voltage - 0.5) * 100 ; //converting from 10 mv per degree with 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" °C");

  if (temperatureC > 30.00)
  {
    myServo.detach();
 
    delay(2000);
 
    myServo.attach(9);
 
    myServo.write(180);

    delay(2000);

    myServo.detach();

    delay(2000);
 
    myServo.attach(9);

    myServo.write(0);
 
    delay(2000);
  }

  myServo.detach();
  
  delay(1000);
}

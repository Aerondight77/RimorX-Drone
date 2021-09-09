//#include <dht.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <WiFiUdp.h>
#include "DHT.h"
#include "TinyGPS++.h"

const char* ssid = "SpectrumSetup-70";  // Enter SSID here
const char* password = "scarypant764";  //Enter Password here

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

ESP8266WebServer server(80);

// DHT Sensor
uint8_t DHTPin = D6; 
#define MQ2pin (A0)
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
TinyGPSPlus gps;
SoftwareSerial ss(4, 5); // MAKE SURE TO REPLACE (rxPin, txPin)!!!!
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

float Temperature, Humidity, MQ2value, latitude, longitude;
String lat_str, lng_str, dayStamp;

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);  // Can it be used in 9600?????
  delay(100);
  
  pinMode(DHTPin, INPUT);
  pinMode(MQ2pin, INPUT);

  dht.begin();              

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  timeClient.begin();
  timeClient.setTimeOffset(-25200); // IF TIME IS NOT CORRECT, CHANGE TO +25200 OR +/- 28800
  Serial.println("HTTP server started");
  delay(20000);  // Added delay for MQ-2.
}

void loop()
{
  while (ss.available() > 0) 
  { //while data is available
    if (gps.encode(ss.read())) //read gps data
    {
      if (gps.location.isValid()) //check whether gps location is valid
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6); // latitude location is stored in a string
        longitude = gps.location.lng();
        lng_str = String(longitude , 6); //longitude location is stored in a string
      }
    }
  }
  
  while(!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  server.handleClient();
}


void handle_OnConnect() {

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  MQ2value = analogRead(MQ2pin); // Gets the values of the gas sensor.
  server.send(200, "text/html", SendHTML(Temperature,Humidity,MQ2value,lat_str,lng_str,dayStamp)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat,float Humiditystat,float GasStat,String latData,String lngData,String dayStr){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 NodeMCU Data Report</h1>\n";
  
  ptr +="<p>Temperature: ";
  ptr +=(int)Temperaturestat;
  ptr +="°C</p>";     // FOR SOME REASON displaying weird A with accent on top of it???!!!?!?!?!?!??
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  ptr +="<p>Gas Concentration: ";
  ptr +=(int)GasStat;
  ptr +=" ppm</p>";

  // GPS DATA
  ptr += "<p>LAT: ";
  ptr += latData;
  ptr += "</p>";
  ptr += "<p>LNG: ";
  ptr += lngData;
  ptr += "</p>";

  // THIS IS TO DISPLAY TIME AND DATE.
  ptr += "<p>Time: ";
  ptr += timeClient.getFormattedTime();
  ptr += "</p>";
  ptr += "<p>Current date: ";
  ptr += dayStr;
  ptr += "</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

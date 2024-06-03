//Blynk Authentications
//Replace the id and token 
#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Battery Charging Indicator"
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
 
//Think Speak Authentication 
//Repalce wifi name,password and api-key
String apiKey = "api-key";
const char* ssid =  "wifi-name";     // Enter your WiFi Network's SSID
const char* pass =  "password"; // Enter your WiFi Network's Password
const char* server = "api.thingspeak.com";
 

//Blynk
char Ssid[] = "wifi-name";  
char Pass[] = "password"; 

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial



int analogInPin  = A0;    // Analog input pin
int sensorValue;          // Analog Output of Sensor
float calibration = 1.6; // Check Battery voltage using multimeter & add/subtract the value
int bat_percentage;
 
WiFiClient client; 

void setup()
{ 
 Serial.begin(115200);
  
  Serial.println("Begin");
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, Ssid, Pass);
}
 

void loop()
{
  Blynk.run();

  sensorValue = analogRead(analogInPin);
  Serial.println(sensorValue);
  float voltage = (((sensorValue * 3.23) / 1023) *2) - calibration;  //multiply by two as voltage divider network is 100K & 100K Resistor
  bat_percentage = mapfloat(voltage, 2.75, 4.2, 0.0, 100.0); //2.75V as Battery Cut off Voltage & 4.2V as Maximum Voltage

 
  if (bat_percentage >= 100) 
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
 

  Serial.print("Analog Value = ");
  Serial.print(sensorValue);
  Serial.print("\t Output Voltage = ");
  Serial.print(voltage);
  Serial.print("\t Battery Percentage = ");
  Serial.println(bat_percentage);
  delay(1000);

//Sending Data to Blynk app 
 Blynk.virtualWrite(V0, voltage);  // for battery voltage
 Blynk.virtualWrite(V1, bat_percentage);  // for battery percentage

 if (bat_percentage <=30)
    {
      Serial.println("Battery level below 30%, Charge battery on time");
      //send notification
      Blynk.logEvent("battery_low", "Battery is getting low.... Plugin to charge") ;
      delay(500);
    }

     if (bat_percentage >=90)
    {
      Serial.println("Battery is fully charged");
      //send notification
      Blynk.logEvent("Battery Charged") ;
      delay(500);
    }

//Sending data to Think speak server
 if (client.connect(server, 80))
  {
 
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(voltage);
    postStr += "&field2=";
    postStr += String(bat_percentage);
    postStr += "&field3=";
    postStr += String(bat_percentage);
   
    postStr += "\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    delay(100);
    client.print("Host: api.thingspeak.com\n");
    delay(100);
    client.print("Connection: close\n");
    delay(100);
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    delay(100);
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    delay(100);
    client.print("Content-Length: ");
    delay(100);
    client.print(postStr.length());
    delay(100);
    client.print("\n\n");
    delay(100);
    client.print(postStr);
    delay(100);
  }
  client.stop();
  Serial.println("Sending....");
  delay(15000);
}

 float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

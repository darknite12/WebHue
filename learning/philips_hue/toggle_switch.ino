/* Project name: ESP8266 - Philips® hue Switch
   Project URI:  https://www.studiopieters.nl/esp8266-philips-hue-switch/
   Description: Switch to use with the Philips® Hue bridge
   Version: 2.3.3
   License: MIT Copyright 2019 StudioPieters®
*/

#include <ESP8266WiFi.h>
#include <SPI.h>

IPAddress ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

int pin_switch = 16;

boolean oldSwitchState = LOW;
boolean newSwitchState1 = LOW;
boolean newSwitchState2 = LOW;
boolean newSwitchState3 = LOW;

int light = 10;                                   // Number of the Hue light to be switched

const char hueHubIP[] = "hubip";       // Hue hub IP
const char hueUsername[] = "username";  // hue bridge username

const int hueHubPort = 80;


const char ssid[] = "ssid";                      // Network SSID (name)
const char pass[] = "password";                  // Network password



boolean onOffState = false;                       // To store actual on/off state of lights as reported by Hue bridge

WiFiClient client;


void getHue()
{
  if (client.connect(hueHubIP, hueHubPort))
  {
    client.print("GET /api/");
    client.print(hueUsername);
    client.print("/lights/");
    client.print(light);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(hueHubIP);
    client.println("Content-type: application/json");
    client.println("keep-alive");
    client.println();
    while (client.connected())
    {
      if (client.available())
      {
        client.findUntil("\"on\":", "\0");
        onOffState = (client.readStringUntil(',') == "true");
        break;
      }
    }
    client.stop();
  }
}




void setHue(String command)
{
  if (client.connect(hueHubIP, hueHubPort))
  {
    client.print("PUT /api/");
    client.print(hueUsername);
    client.print("/lights/");
    client.print(light);
    client.println("/state HTTP/1.1");
    client.println("keep-alive");
    client.print("Host: ");
    client.println(hueHubIP);
    client.print("Content-Length: ");
    client.println(command.length());
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println();                             // Blank line before body
    client.println(command);
    client.stop();
  }
}




void setup()
{


  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1);

  }

  pinMode(pin_switch, INPUT); 

}



void loop()
{

  getHue();                                       // Lights 1 status

  newSwitchState1 = digitalRead(pin_switch);
  delay(1);
  newSwitchState2 = digitalRead(pin_switch);
  delay(1);
  newSwitchState3 = digitalRead(pin_switch);
 
  // if all 3 values are the same we can continue
  if (  (newSwitchState1==newSwitchState2) && (newSwitchState1==newSwitchState3) ) {
    if ( newSwitchState1 != oldSwitchState ) {
      // has the button switch been closed?
      if ( newSwitchState1 == HIGH ) {
        if (onOffState == true) {
          String command =  "{\"on\": false}";
          setHue(command);
        }
        else {
          String command =  "{\"on\": true}";
          setHue(command); 
        }
      }
      oldSwitchState = newSwitchState1;
    }  
  }
}

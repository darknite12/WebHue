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
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long times = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

int light = 10;                                   // Number of the Hue light to be switched

const char hueHubIP[] = "192.168.1.135";       // Hue hub IP
const char hueUsername[] = "8GVlH915yoo5QtBGFOiSvcIk8HzOWj5DgUPcpVTU";  // hue bridge username

const int hueHubPort = 80;


const char ssid[] = "tortuga";                      // Network SSID (name)
const char pass[] = "dos gardenias para ti";                  // Network password



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

  


  reading = digitalRead(pin_switch);

  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == HIGH && previous == LOW && millis() - times > debounce) {
    if (state == HIGH) {
      state = LOW;
      if (onOffState == true) {
        String command =  "{\"on\": false}";
        setHue(command);
      } else {
          String command =  "{\"on\": true}";
          setHue(command); 
        }
    }
    else {
      state = HIGH;
      if (onOffState == true) {
        String command =  "{\"on\": false}";
        setHue(command);
      } else {
          String command =  "{\"on\": true}";
          setHue(command); 
        }
      
    }
    times = millis();    
  }
  previous = reading;
}
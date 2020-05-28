
#include <ESP8266WiFi.h>
#include <SPI.h>

IPAddress ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char ssid[] = "ssid";                      // Network SSID (name)
const char pass[] = "password";        // Network password

int buttonPin = 16;
int buttonNew;
int buttonOld = 1;
int dt = 100;

const char hueHubIP[] = "hue_hub_ip";
const char hueUsername[] = "hue_user_name";
const int hueHubPort = 80;
int light = 10;           // Number of the Hue light to be switched
boolean onOffState = false;

WiFiClient client;
void getHue() {
  if (client.connect(hueHubIP, hueHubPort)) {
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
    while (client.connected()) {
      if (client.available()) {
        client.findUntil("\"on\":", "\0");
        onOffState = (client.readStringUntil(',') == "true");
        break;
      }
    }
    client.stop();
  }
}

void setHue(String command) {
  if (client.connect(hueHubIP, hueHubPort)) {
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

void setup() {
  Serial.begin(9600);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  pinMode(buttonPin, INPUT); 
}

void loop() {
  buttonNew = digitalRead(buttonPin);
  if(buttonOld == 0 && buttonNew == 1) {
    String command = "";
    getHue();     // Get lights status
    Serial.println(onOffState);
    if (onOffState == true)  {
      command =  "{\"on\": false}";
    } else {
      command =  "{\"on\": true}";
    }
    Serial.println(command);
    setHue(command);
  }
  buttonOld = buttonNew;
  delay(dt);
}

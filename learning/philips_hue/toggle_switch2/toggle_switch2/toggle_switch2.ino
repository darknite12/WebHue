#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


IPAddress ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

const char ssid[] = "tortuga";                      // Network SSID (name)
const char pass[] = "dos gardenias para ti";        // Network password

int buttonPin = 16;
int buttonNew;
int buttonOld = 1;
int dt = 100;

const char hueHubIP[] = "192.168.1.135";
const char hueUsername[] = "8GVlH915yoo5QtBGFOiSvcIk8HzOWj5DgUPcpVTU";
const int hueHubPort = 80;
int light = 10;           // Number of the Hue light to be switched

boolean isOn(int lightId) {
  boolean isOn = false;  
  HTTPClient http;
  String req_string;
  req_string = "http://";
  req_string += hueHubIP;
  req_string += "/api/";
  req_string += hueUsername;
  req_string += "/lights/";
  req_string += lightId;
  http.begin(req_string);
  http.addHeader("Content-Type", "text/plain");
  
  int httpResponseCode = http.GET();
  if(httpResponseCode > 0){
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, http.getStream());
    isOn = doc["state"]["on"].as<String>() == "true";
    Serial.print("GET: ");
    Serial.println(httpResponseCode);         
   } else {
    Serial.print("Error on sending GET Request: ");
    Serial.println(httpResponseCode);
    Serial.print("WiFi Status");
    Serial.println(WiFi.status());
   }
   http.end();
   return isOn;
}

void setHue(String command) {
  HTTPClient http; 
  String req_string;
  req_string = "http://";
  req_string += hueHubIP;
  req_string += "/api/";
  req_string += hueUsername;
  req_string += "/lights/";
  req_string += light;
  req_string += "/state";
  http.begin(req_string);
  http.addHeader("Content-Type", "text/plain");
  
  int httpResponseCode = http.PUT(command);
  
  if(httpResponseCode > 0){
    String response = http.getString();
    Serial.print("PUT: "); 
    Serial.println(httpResponseCode);          
   } else {
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
   }
   http.end();
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
    if (isOn(light))  {
      setHue("{\"on\": false}");
    } else {
      setHue("{\"on\": true}");
    }
  }
  buttonOld = buttonNew;
  delay(dt);
}

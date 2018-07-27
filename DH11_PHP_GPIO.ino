#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN D1
#define DHTTYPE DHT11 
 
const char* ssid     = "linksys";
const char* password = "secretcube";
const char* host = "iot9641.000webhostapp.com";
String url;
  int count = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);
  dht.begin(); // DHT11 config

  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D0, 0);
  digitalWrite(D2, 0);
  digitalWrite(D3, 0);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}


void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  String url = "/api/weather/insert.php?temp=" + String(t) + "&hum="+ String(h);
  Serial.print("Requesting URL: ");
  Serial.println(url);


  // GPIO Control

   if (count == 0){
    url = "/read_all.php?id=1";
    count = count + 1;
    Serial.println("Here1");
  }
  else if (count == 1){
    url = "/read_all.php?id=2";
    count = count + 1;
    Serial.println("Here2");
  }
  else if (count == 2){
    url = "/read_all.php?id=3";
    count = count + 1;
    Serial.println("Here3");
  }
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
     
      if (line=="\n") { // skips the empty space at the beginning 
        section="json";
      }
    }
    else if (section=="json") {  // print the good stuff
      section="ignore";
      String result = line.substring(1);

      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
      String led = json_parsed["led"][0]["status"];

      if(count == 1) {
        if(led == "on"){
          digitalWrite(D0, 1);
          delay(100);
          Serial.println("D1 is On..!");
        }
        else if(led == "off"){
          digitalWrite(D0, 0);
          delay(100);
          Serial.println("D1 is Off..!");
        }
      }


      
      else if(count == 2){
        if(led == "on"){
          digitalWrite(D2, 1);
          Serial.println("D2 is On..!");
        }
        else if(led == "off"){
          digitalWrite(D2, 0);
          Serial.println("D2 is Off..!");
        }
      }


      
      else if(count == 3){
        if(led == "on"){
          digitalWrite(D3, 1);
          Serial.println("D3 is On..!");
        }
        else if(led == "off"){
          digitalWrite(D3, 0);
          Serial.println("D3 is Off..!");
        }
        count = 0;
      }

      if (count == 3)
        count = 0;


    }}
    


 
  delay(500);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  delay(3000);
}


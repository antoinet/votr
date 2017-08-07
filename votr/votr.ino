/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Ticker.h>

// wifi constants
const char* ssid = "esp8266net";
const char* password = "hackathon";

void update_presence();
void button_pressed();
bool btn_pressed = false;

void setup() {
  // Setup serial 
  Serial.begin(115200);
  Serial.println();

  // Setup wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup button handler
  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), button_pressed, RISING);
}




void button_pressed() {
  btn_pressed = true;
}

void loop() {
  delay(500);
  if (btn_pressed) {
    Serial.println("button pressed!");
    HTTPClient http;
    char *url = "http://192.168.0.2/vote";
    
    Serial.printf("[HTTP] sending vote to: '%s'\n", url);
    http.begin(url);
    
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("[HTTP] 200 OK, vote sent.");  
    } else {
      Serial.printf("[HTTP] GET... failed, error %d: %s\n", httpCode, http.errorToString(httpCode).c_str());
    }
    http.end();
    btn_pressed = false;
  }
}

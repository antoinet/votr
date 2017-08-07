#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IPAddress.h>
#include <FS.h>

const char* ssid = "esp8266net";
const char* password = "hackathon";

ESP8266WebServer server(80);

const int led = 16;

#define MAX_ADDRESSES 64
int votes = 0;
IPAddress addresses[MAX_ADDRESSES];

bool handleRoot() {
  digitalWrite(led, 1);
  if (SPIFFS.exists("/index.html")) {
    File file = SPIFFS.open("/index.html", "r");
    size_t sent = server.streamFile(file, "text/html");
    file.close();
    digitalWrite(led, 0);
    return true;
  }
  digitalWrite(led, 0);
  return false;
}


void handleVote() {
  digitalWrite(led, 1);

  int i;
  IPAddress peeraddr = server.client().remoteIP();
  
  for (i = 0; i < votes && i < MAX_ADDRESSES; i++) {
    if (peeraddr == addresses[i]) {
      Serial.println(peeraddr.toString() + ": already voted\n");
      server.send(200, "text/plain", "already voted");
      digitalWrite(led, 0);
      return;
    }
  }
  
  if (i >= MAX_ADDRESSES) {
    Serial.println("Error: MAX_ADDRESSES reached\n");
    server.send(500, "text/plain", "maximum participants reached");
    digitalWrite(led, 0);
    return;
  }

  Serial.println(peeraddr.toString() + ": voted\n");
  addresses[i] = peeraddr;
  votes++;
  server.send(200, "text/plain", "vote registered");
  digitalWrite(led, 0);
}


void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("votr")) {
    Serial.println("MDNS responder started");
  }

  SPIFFS.begin();

  server.on("/", handleRoot);
  server.on("/vote", handleVote);
  server.on("/getvotes", [](){
    server.send(200, "text/plain", String(votes));
  });
  server.onNotFound([](){
    server.send(404, "text/plain", "Not Found");
  });

  server.begin();
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);
}

void loop(void){
  server.handleClient();
}

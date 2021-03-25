#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;

//#define Serial Serial1

WebSocketsServer webSocket = WebSocketsServer(80);

String IpAddress2String(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + \
         String(ipAddress[1]) + String(".") + \
         String(ipAddress[2]) + String(".") + \
         String(ipAddress[3])  ;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.print("dt");
    Serial.print((char*)payload);
    Serial.print("\r");
  }

}

void setup() {
  // USE_SERIAL.begin(921600);
  Serial.begin(115200);

  WiFiMulti.addAP("Cuba_2G4", "Cube09021922211605");
  WiFiMulti.addAP("KVANT72", "28346dGb7");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  String ardudata = "noth";
  if (Serial.available()) ardudata = Serial.readStringUntil('\r');
  if (ardudata == "getIP") {
    bool is_connected = (WiFiMulti.run() == WL_CONNECTED);
    Serial.print(is_connected ? "ipConnected\n" : "ipDisconnected\n");
    Serial.print(is_connected ? IpAddress2String(WiFi.localIP()) : " ");
    Serial.print("\r");
  } else if (ardudata == "noth") {
    digitalWrite(3, true);
  } else webSocket.broadcastTXT(ardudata);
  webSocket.loop();
}

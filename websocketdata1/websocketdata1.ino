#include <SPI.h>
#include <Ethernet.h>

// Enabe debug tracing to Serial port.
#define DEBUG

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64

#include <WebSocket.h>

byte mac[] = { 0x52, 0x4F, 0x43, 0x4B, 0x45, 0x54 };
byte ip[] = { 192, 168, 1 , 77 };

// Create a Websocket server
WebSocketServer wsServer;

unsigned long starttime = 0;

bool is_ready = true;
bool prev_ready = false;

void onConnect(WebSocket &socket) {
  Serial.println("onConnect called");
}


// You must have at least one function with the following signature.
// It will be called by the server when a data frame is received.
void onData(WebSocket &socket, char* dataString, byte frameLength) {
  
#ifdef DEBUG
  Serial.print("Got data: ");
  Serial.write((unsigned char*)dataString, frameLength);
  Serial.println();
#endif
  
  // Just echo back data for fun.
  socket.send(dataString, strlen(dataString));
}

void onDisconnect(WebSocket &socket) {
  Serial.println("onDisconnect called");
}

void setup() {
#ifdef DEBUG  
  Serial.begin(115200);
#endif
  Ethernet.begin(mac, ip);
  Serial.println(Ethernet.localIP());
  
  wsServer.registerConnectCallback(&onConnect);
  wsServer.registerDataCallback(&onData);
  wsServer.registerDisconnectCallback(&onDisconnect);  
  wsServer.begin();
  
  delay(100); // Give Ethernet time to get ready
}

void loop() {
  // Should be called for each loop.
  wsServer.listen();
  
  // Do other stuff here, but don't hang or cause long delays.
  if (wsServer.connectionCount() > 0) {
    wsServer.send("abc123", 6);
  }
  if (!is_ready) {
    digitalWrite(5, true);
    digitalWrite(6, false);
    Serial.print("initializing scan");
    delay(500);
    Serial.print(".");
    delay(500);
    Serial.print(".");
    delay(500);
    Serial.println(".");
    delay(500);
    if (prev_ready) starttime = millis();
    float time_left = float(starttime + 30000 - millis()) / 1000;
    if (millis() <= starttime + 30000) {
      //String data = "lol";
      //analogRead(A0);
      //analogRead(A1);

      Serial.print("time remaining: ");
    } else is_ready = true;
  } else {
    digitalWrite(6, true);
    digitalWrite(5, false);
    if (!prev_ready) {
      Serial.println("ready");
    }
    String in = Serial.readString();
    if (in == "1") {
      is_ready = false;
    }
    //is_ready = ! digitalRead(8);
    prev_ready = is_ready;
  }
  delay(50);
}

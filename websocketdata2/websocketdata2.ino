#include <SPI.h>
#include <Ethernet.h>
#include <WebSocket.h>

byte mac[] = { 0x69, 0xFF, 0x69, 0xFF, 0x69, 0xFF };

WebSocketServer wsServer;

unsigned long bpmstarttime = 0;
unsigned long ecgstarttime = 0;

unsigned long bpm_lastbeattime = 0;
unsigned long bpm_lastlastbeattime = 0;

bool bpmbusy = false;
bool ecgbusy = false;
bool bpmbusy_prev = false;
bool ecgbusy_prev = false;

bool bpm_state = false;
bool bpm_state_prev = false;

word ecgdata[100];
byte ecgdatapos = 0

void setup() {
  Ethernet.begin(mac);

  wsServer.registerConnectCallback(&onConnect);
  wsServer.registerDataCallback(&onData);
  wsServer.registerDisconnectCallback(&onDisconnect);
  wsServer.begin();
}

void loop() {
  unsigned long curtime = millis();
  wsServer.listen();
  if (wsServer.connectionCount() > 0) {
    wsServer.send("abc123", 6);
  }
  if (bpmbusy) {
    if (!bpmbusy_prev) bpmstarttime = curtime;
    if (curtime > bpmstarttime + 30000) bpmbusy = false;
    bpm_state_prev = bpm_state;

    bpm_state = analogRead(A0) > 512 ? true : false;
    if (bpm_state && !bpm_state_prev) {
      bpm_lastlastbeattime = bpm_lastbeattime;
      bpm_lastbeattime = curtime;
    }
  }
  if (ecgbusy) {
    if (!ecgbusy_prev) ecgstarttime = curtime;
    if (curtime > ecgstarttime + 30000) ecgbusy = false;

    ecgdatapos = ecgdatapos == 99 ? 0 : ecgdatapos + 1;
    ecgdata[ecgdatapos] = 
  }
  delay(20);
}

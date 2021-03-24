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
byte ecgdatapos = 0;

void onConnect(WebSocket &socket) {

}

void onData(WebSocket &socket, char* dataString, byte frameLength) {
  if (String(dataString) == "startbpm") bpmbusy = true;
  if (String(dataString) == "stopbpm") bpmbusy = false;
  if (String(dataString) == "startecg") ecgbusy = true;
  if (String(dataString) == "stopecg") ecgbusy = false;
}

void onDisconnect(WebSocket &socket) {

}

String wordarraytoString(word in[100]) {
  String out = "";
  for (byte i = 0; i < 100; i++) {
    out += String(in[i]);
    if (i != 99) out += ",";
  }
  return out;
}

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
    ecgdata[ecgdatapos] = (digitalRead(A2) == true && digitalRead(A3) == true) ? 0 : analogRead(A1);
  }

  if (wsServer.connectionCount() > 0) {
    String out = "";
    out += "{\"bpm_data\":{\"lastbeatinterval\":";
    out += String(bpm_lastbeattime - bpm_lastlastbeattime);
    out += ",\"time_left\":";
    out += String(bpmstarttime + 30000 - curtime);
    out += ",\"is_busy\":\"";
    out += String(bpmbusy);

    out += "\"},\"ecg_data\":{\"data\":\"";
    out += wordarraytoString(ecgdata);
    out += "\",\"datapos\":";
    out += String(ecgdatapos);
    out += "\",\"time_left\":";
    out += String(ecgstarttime + 30000 - curtime);
    out += ",\"is_busy\":\"";
    out += String(ecgbusy);

    out += "\"}";

    char outarr[out.length()];
    out.toCharArray(outarr, out.length());
    wsServer.send(outarr, out.length());
  }
  delay(20);
}

#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

unsigned long showipstarttime = 0;
bool showip = false;
bool showip_prev = false;
String ip = "Disconnected\n0.0.0.0";

unsigned long bpmstarttime = 0;
unsigned long ecgstarttime = 0;
bool bpmbusy = false;
bool ecgbusy = false;
bool bpmbusy_prev = false;
bool ecgbusy_prev = false;

unsigned long bpm_lastbeattime = 0;
unsigned long bpm_lastlastbeattime = 0;
bool bpm_state = false;
bool bpm_state_prev = false;

word ecgdata[10];
byte ecgdatapos = 0;

String wordarraytoString(word in[10]) {
  String out = "";
  for (byte i = 0; i < 10; i++) {
    out += String(in[i]);
    if (i != 9) out += ",";
  }
  return out;
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("initializing...");
  Serial.println("starting things up");

  lcd.clear();
  showip = true;
}

void loop() {
  String espdata = "noth";
  if (Serial.available()) espdata = Serial.readStringUntil('\r');
  if (espdata.startsWith("dt")) {
    if (espdata.endsWith("startbpm")) bpmbusy = true;
    if (espdata.endsWith("stopbpm")) bpmbusy = false;
    if (espdata.endsWith("startecg")) ecgbusy = true;
    if (espdata.endsWith("stopecg")) ecgbusy = false;
  } else if (espdata.startsWith("ip"))
    ip = espdata.substring(2);

  unsigned long curtime = millis();
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

    ecgdatapos = ecgdatapos == 9 ? 0 : ecgdatapos + 1;
    ecgdata[ecgdatapos] = (digitalRead(A2) == true && digitalRead(A3) == true) ? 0 : analogRead(A1);
  }

  if (digitalRead(A4)) showip = true;

  if (showip) {
    if (!showip_prev) {
      showipstarttime = curtime;
      Serial.print("getIP\r");
    }
    if (curtime > showipstarttime + 5000) showip = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ip.substring(0, ip.indexOf('\n')));
    lcd.setCursor(0, 1);
    lcd.print(ip.substring(ip.indexOf('\n') + 1));
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM " + (bpmbusy ? "RUN" + String((byte)(bpmstarttime + 30000 - curtime)) : "STB"));
    lcd.setCursor(0, 1);
    lcd.print("ECG " + (bpmbusy ? "RUN" + String((byte)(bpmstarttime + 30000 - curtime)) : "STB"));
  }

  String out = "";
  out += "{\"curtime\":";
  out += String(curtime);
  
  out += ",\"bpm_data\":{\"lastbeatinterval\":";
  out += String(bpm_lastbeattime - bpm_lastlastbeattime);
  out += ",\"time_left\":";
  out += String(bpmstarttime);
  out += ",\"is_busy\":\"";
  out += String(bpmbusy);
  out += "\"},\"ecg_data\":{\"data\":[";
  out += wordarraytoString(ecgdata);
  out += "],\"datapos\":";
  out += String(ecgdatapos);
  out += ",\"time_left\":";
  out += String(ecgstarttime);
  out += ",\"is_busy\":\"";
  out += String(ecgbusy);
  out += "\"}}\r";

  Serial.print(out);

  digitalWrite(8, bpmbusy || ecgbusy);
  digitalWrite(9, (!bpmbusy) && (!ecgbusy));

  delay(100);

  bpmbusy_prev = bpmbusy;
  ecgbusy_prev = ecgbusy;
  showip_prev = showip;
}

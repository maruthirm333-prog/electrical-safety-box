#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
WebServer server(80);

// ---------------- PINS ----------------
const int GREEN_LED = 14;
const int RED_LED   = 27;
const int BUZZER    = 25;
const int BUTTON    = 32;
const int NTC1_PIN  = 34;
const int NTC2_PIN  = 35;
const int RELAY     = 33;

// ---------------- WIFI ----------------
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ---------------- NTC ----------------
const float R_FIXED    = 10000.0;
const float R0         = 10000.0;
const float BETA       = 3950.0;
const float TEMP_REF_K = 298.15;

// ---------------- PROTECTION ----------------
const float DEMO_TRIP_TEMP = 35.0;
const float RESET_TEMP     = 30.0;

// ---------------- STATE ----------------
float temp1 = NAN, temp2 = NAN, deltaT = NAN;
float voltageV = 0, currentA = 0, powerW = 0;
float healthIndex = NAN;

bool fault = false, relayState = false;
unsigned long applianceStartTime = 0, totalRunTime = 0;
unsigned long lastSensorRead = 0, lastBlink = 0;
bool greenState = false;


// ── NTC READING ───────────────────────────────────────────────
float readNTC(int pin) {
  int adcValue = analogRead(pin);
  float voltage = (adcValue / 4095.0) * 3.3;
  if (voltage <= 0.01 || voltage >= 3.29) return NAN;
  float rNtc = R_FIXED * voltage / (3.3 - voltage);
  float tempK = 1.0 / ((1.0 / TEMP_REF_K) + (log(rNtc / R0) / BETA));
  return tempK - 273.15;
}


// ── RELAY ─────────────────────────────────────────────────────
void relayON() {
  digitalWrite(RELAY, LOW);                   // active LOW relay
  if (!relayState) applianceStartTime = millis();
  relayState = true;
}

void relayOFF() {
  digitalWrite(RELAY, HIGH);
  if (relayState) totalRunTime += millis() - applianceStartTime;
  relayState = false;
}


// ── FAULT / RESET ─────────────────────────────────────────────
void triggerFault() {
  fault = true;
  relayOFF();
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  Serial.println("FAULT - POWER CUT");
}

void resetFault() {
  if (!isnan(temp1) && temp1 <= RESET_TEMP) {
    fault = false;
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
    relayON();
    Serial.println("SYSTEM RESET");
  } else {
    Serial.println("RESET REFUSED - STILL HOT");
  }
}


// ── WEB DASHBOARD ─────────────────────────────────────────────
String webpage() {
  return R"rawliteral(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Electrical Safety</title>
<style>
body{font-family:Arial;background:#f4f6f8;margin:0;padding:20px}
.card{max-width:500px;margin:auto;background:white;padding:20px;border-radius:15px;box-shadow:0 4px 15px rgba(0,0,0,0.1)}
h1{text-align:center}.value{font-size:25px;font-weight:bold}
.status{padding:12px;border-radius:10px;text-align:center;margin:15px 0}
button{width:48%;padding:14px;border:none;border-radius:10px;font-size:16px;margin-top:10px}
.on{background:#198754;color:white}.off{background:#dc3545;color:white}
.row{display:flex;justify-content:space-between;padding:8px 0;border-bottom:1px solid #ddd}
</style></head><body>
<div class="card">
<h1>⚡ Smart Safety</h1>
<div id="status" class="status">Loading...</div>
<div class="row"><span>Appliance</span><span>FAN</span></div>
<div class="row"><span>Voltage</span><span id="voltage">--</span></div>
<div class="row"><span>Current</span><span id="current">--</span></div>
<div class="row"><span>Power</span><span id="power">--</span></div>
<div class="row"><span>Connection Temp</span><span id="temp1">--</span></div>
<div class="row"><span>Room Temp</span><span id="temp2">--</span></div>
<div class="row"><span>Delta T</span><span id="delta">--</span></div>
<div class="row"><span>Health Index</span><span id="health">--</span></div>
<div class="row"><span>Runtime</span><span id="runtime">--</span></div>
<button class="on" onclick="controlRelay('on')">TURN ON</button>
<button class="off" onclick="controlRelay('off')">TURN OFF</button>
<button style="width:100%;background:#ffc107;" onclick="resetSystem()">RESET</button>
</div>
<script>
function updateData(){
  fetch('/data').then(r=>r.json()).then(data=>{
    document.getElementById("voltage").innerText=data.voltage+" V";
    document.getElementById("current").innerText=data.current+" A";
    document.getElementById("power").innerText=data.power+" W";
    document.getElementById("temp1").innerText=data.temp1+" °C";
    document.getElementById("temp2").innerText=data.temp2+" °C";
    document.getElementById("delta").innerText=data.delta+" °C";
    document.getElementById("health").innerText=data.health;
    document.getElementById("runtime").innerText=data.runtime;
    let s=document.getElementById("status");
    if(data.fault){s.innerText="🚨 FAULT - POWER OFF";s.style.background="#f8d7da";}
    else if(data.relay){s.innerText="🟢 APPLIANCE ON";s.style.background="#d1e7dd";}
    else{s.innerText="⚪ APPLIANCE OFF";s.style.background="#e2e3e5";}
  });
}
function controlRelay(a){fetch("/relay?action="+a).then(()=>updateData());}
function resetSystem(){fetch("/reset").then(()=>updateData());}
setInterval(updateData,1000);updateData();
</script></body></html>
)rawliteral";
}


// ── WEB ROUTES ────────────────────────────────────────────────
void handleRoot() { server.send(200,"text/html",webpage()); }

void handleData() {
  String json="{";
  json+=""voltage":"+String(voltageV,2)+",";
  json+=""current":"+String(currentA,3)+",";
  json+=""power":"+String(powerW,2)+",";
  json+=""temp1":"+String(temp1,2)+",";
  json+=""temp2":"+String(temp2,2)+",";
  json+=""delta":"+String(deltaT,2)+",";
  if(isnan(healthIndex)) json+=""health":"Waiting for >=1A",";
  else json+=""health":""+String(healthIndex,3)+"",";
  unsigned long runtime=totalRunTime;
  if(relayState) runtime+=millis()-applianceStartTime;
  unsigned long s=runtime/1000;
  char rt[20]; sprintf(rt,"%02d:%02d:%02d",(int)(s/3600),(int)((s%3600)/60),(int)(s%60));
  json+=""runtime":""; json+=rt; json+="",";
  json+=""relay":"; json+=relayState?"true":"false"; json+=",";
  json+=""fault":"; json+=fault?"true":"false"; json+="}";
  server.send(200,"application/json",json);
}

void handleRelay() {
  if(server.hasArg("action")){
    String a=server.arg("action");
    if(a=="on"&&!fault) relayON();
    else if(a=="off") relayOFF();
  }
  server.send(200,"text/plain","OK");
}

void handleReset() { resetFault(); server.send(200,"text/plain","RESET CHECKED"); }


// ── SETUP ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED,OUTPUT); pinMode(RED_LED,OUTPUT);
  pinMode(BUZZER,OUTPUT);    pinMode(BUTTON,INPUT_PULLUP);
  pinMode(RELAY,OUTPUT);     relayOFF();
  analogReadResolution(12);  analogSetAttenuation(ADC_11db);

  if(!ina219.begin()){ Serial.println("INA219 NOT FOUND"); while(1) delay(100); }

  WiFi.begin(ssid,password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status()!=WL_CONNECTED){ delay(500); Serial.print("."); }
  Serial.println(); Serial.println("WiFi connected");
  Serial.print("Open: "); Serial.println(WiFi.localIP());

  server.on("/",handleRoot); server.on("/data",handleData);
  server.on("/relay",handleRelay); server.on("/reset",handleReset);
  server.begin();
  Serial.println("WEB SERVER STARTED");
  relayON();
}


// ── LOOP ──────────────────────────────────────────────────────
void loop() {
  server.handleClient();

  // Heartbeat LED
  if(!fault && millis()-lastBlink>=500){
    lastBlink=millis(); greenState=!greenState;
    digitalWrite(GREEN_LED,greenState);
  }

  // Sensors every 1 second
  if(millis()-lastSensorRead>=1000){
    lastSensorRead=millis();
    temp1=readNTC(NTC1_PIN); temp2=readNTC(NTC2_PIN);
    voltageV=ina219.getBusVoltage_V();
    currentA=ina219.getCurrent_mA()/1000.0;
    powerW=ina219.getPower_mW()/1000.0;
    if(!isnan(temp1)&&!isnan(temp2)) deltaT=temp1-temp2;
    healthIndex=(currentA>=1.0)?(deltaT/(currentA*currentA)):NAN;
    if(!fault&&!isnan(temp1)&&temp1>=DEMO_TRIP_TEMP) triggerFault();
  }

  // Physical reset button
  if(digitalRead(BUTTON)==LOW){
    delay(30);
    if(digitalRead(BUTTON)==LOW){
      resetFault();
      while(digitalRead(BUTTON)==LOW) delay(10);
    }
  }
}

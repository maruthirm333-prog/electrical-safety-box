#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <math.h>

// =====================================================
// WIFI
// =====================================================
const char* WIFI_SSID     = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =====================================================
// PIN DEFINITIONS
// =====================================================
#define NTC1_PIN       34    // Connection temperature
#define NTC2_PIN       35    // Ambient / room temperature

#define SDA_PIN        21
#define SCL_PIN        22

#define RELAY_PIN      33

#define GREEN_LED_PIN  14
#define RED_LED_PIN    27
#define BUZZER_PIN     25

#define RESET_BUTTON   32
#define ISD_TRIGGER    26    // ISD1820 voice playback trigger

// =====================================================
// NTC SETTINGS (B3950, 10k)
// =====================================================
const float R_FIXED    = 10000.0;
const float R0         = 10000.0;
const float BETA       = 3950.0;
const float TEMP_REF_K = 298.15;

// =====================================================
// PROTECTION THRESHOLDS
// =====================================================
const float TRIP_TEMP  = 30.0;   // Trip if NTC-1 >= this
const float RESET_TEMP = 28.0;   // Allow reset only if NTC-1 <= this

// =====================================================
// OBJECTS
// =====================================================
Adafruit_INA219 ina219;
WebServer server(80);

// =====================================================
// STATE
// =====================================================
float temperature1 = 0;
float temperature2 = 0;
float deltaT       = 0;

float voltage    = 0;
float current_mA = 0;
float power_mW   = 0;

bool fault       = false;
bool voicePlayed = false;

unsigned long runtimeStart   = 0;
unsigned long runtimeSeconds = 0;
unsigned long lastStatus     = 0;
unsigned long lastBuzzer     = 0;
bool buzzerState = false;


// =====================================================
// NTC READING
// =====================================================
float readNTC(int pin)
{
  int adc = analogRead(pin);
  float voltageADC = (adc / 4095.0) * 3.3;

  if (voltageADC <= 0.01 || voltageADC >= 3.29) return -999;

  float resistance  = R_FIXED * voltageADC / (3.3 - voltageADC);
  float temperatureK = 1.0 / ((1.0 / TEMP_REF_K) + (log(resistance / R0) / BETA));

  return temperatureK - 273.15;
}


// =====================================================
// RELAY
// =====================================================
void relayON()
{
  digitalWrite(RELAY_PIN, LOW);   // Active LOW relay
  if (runtimeStart == 0) runtimeStart = millis();
}

void relayOFF()
{
  digitalWrite(RELAY_PIN, HIGH);
  if (runtimeStart != 0) {
    runtimeSeconds += (millis() - runtimeStart) / 1000;
    runtimeStart = 0;
  }
}

unsigned long getRuntime()
{
  if (runtimeStart != 0)
    return runtimeSeconds + ((millis() - runtimeStart) / 1000);
  return runtimeSeconds;
}


// =====================================================
// FAULT
// =====================================================
void activateFault()
{
  if (!fault) {
    fault = true;
    relayOFF();
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);

    if (!voicePlayed) {
      digitalWrite(ISD_TRIGGER, HIGH);
      delay(200);
      digitalWrite(ISD_TRIGGER, LOW);
      voicePlayed = true;
    }

    Serial.println("\n!!! FAULT ACTIVATED !!!");
    Serial.println("Relay OFF | Fan OFF | Voice alert played");
  }
}

void resetFault()
{
  if (temperature1 <= RESET_TEMP) {
    fault       = false;
    voicePlayed = false;
    digitalWrite(RED_LED_PIN,   LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    relayON();
    Serial.println("\nFAULT RESET — System NORMAL");
  } else {
    Serial.println("\nRESET REFUSED — Temperature still high");
  }
}


// =====================================================
// BUZZER (1Hz blink on fault)
// =====================================================
void updateBuzzer()
{
  if (fault) {
    if (millis() - lastBuzzer >= 1000) {
      lastBuzzer  = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
  }
}


// =====================================================
// INA219
// =====================================================
void readINA219()
{
  voltage    = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW   = ina219.getPower_mW();
}


// =====================================================
// WEB DASHBOARD
// =====================================================
void handleRoot()
{
  String html = "";
  html += "<!DOCTYPE html><html lang='en'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width,initial-scale=1.0'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>Smart Electrical Safety</title>";
  html += "<style>";
  html += "*{box-sizing:border-box;}";
  html += "body{margin:0;font-family:Arial,Helvetica,sans-serif;background:#f4f6f8;color:#111;}";
  html += ".header{background:#111827;color:white;padding:25px 15px;text-align:center;}";
  html += ".header h1{margin:0;font-size:28px;}";
  html += ".header p{margin:8px 0 0;color:#cbd5e1;}";
  html += ".container{max-width:600px;margin:auto;padding:15px;}";
  html += ".card{background:white;border-radius:18px;padding:22px;margin-bottom:16px;box-shadow:0 3px 12px rgba(0,0,0,0.10);}";
  html += ".card h2{margin-top:0;font-size:22px;}";
  html += ".grid{display:grid;grid-template-columns:1fr 1fr;gap:12px;}";
  html += ".valueBox{background:#f8fafc;border-radius:12px;padding:15px;text-align:center;}";
  html += ".label{font-size:14px;color:#64748b;}";
  html += ".value{font-size:23px;font-weight:bold;margin-top:5px;}";
  html += ".normal{background:#dcfce7;color:#166534;text-align:center;padding:18px;border-radius:14px;font-size:25px;font-weight:bold;}";
  html += ".fault{background:#fee2e2;color:#991b1b;text-align:center;padding:18px;border-radius:14px;font-size:25px;font-weight:bold;}";
  html += ".buttonGrid{display:grid;grid-template-columns:1fr 1fr;gap:10px;}";
  html += "button{width:100%;border:none;padding:16px;border-radius:12px;font-size:17px;font-weight:bold;color:white;}";
  html += ".on{background:#16a34a;}.off{background:#dc2626;}.reset{background:#2563eb;grid-column:1/3;}";
  html += ".small{text-align:center;color:#64748b;font-size:13px;}";
  html += "@media(max-width:400px){.grid{grid-template-columns:1fr;}.buttonGrid{grid-template-columns:1fr;}.reset{grid-column:auto;}}";
  html += "</style></head><body>";

  html += "<div class='header'><h1>Smart Electrical Safety</h1><p>ESP32 Monitoring &amp; Protection System</p></div>";
  html += "<div class='container'>";

  // Electrical params
  html += "<div class='card'><h2>Electrical Parameters</h2><div class='grid'>";
  html += "<div class='valueBox'><div class='label'>Voltage</div><div class='value'>" + String(voltage,2) + " V</div></div>";
  html += "<div class='valueBox'><div class='label'>Current</div><div class='value'>" + String(current_mA,1) + " mA</div></div>";
  html += "<div class='valueBox'><div class='label'>Power</div><div class='value'>" + String(power_mW/1000.0,2) + " W</div></div>";
  html += "<div class='valueBox'><div class='label'>Runtime</div><div class='value'>" + String(getRuntime()) + " s</div></div>";
  html += "</div></div>";

  // Temperature
  html += "<div class='card'><h2>Temperature Monitoring</h2><div class='grid'>";
  html += "<div class='valueBox'><div class='label'>NTC-1 Connection</div><div class='value'>" + String(temperature1,2) + " &deg;C</div></div>";
  html += "<div class='valueBox'><div class='label'>NTC-2 Ambient</div><div class='value'>"    + String(temperature2,2) + " &deg;C</div></div>";
  html += "</div>";
  html += "<div class='valueBox' style='margin-top:12px;'><div class='label'>Temperature Difference (&Delta;T)</div><div class='value'>" + String(deltaT,2) + " &deg;C</div></div>";
  html += "</div>";

  // Status
  html += "<div class='card'>";
  html += fault ? "<div class='fault'>FAULT DETECTED</div>" : "<div class='normal'>NORMAL</div>";
  html += "<div class='grid' style='margin-top:15px;'>";
  html += "<div class='valueBox'><div class='label'>Relay</div><div class='value'>";
  html += (digitalRead(RELAY_PIN) == LOW) ? "ON" : "OFF";
  html += "</div></div>";
  html += "<div class='valueBox'><div class='label'>Runtime</div><div class='value'>" + String(getRuntime()) + " s</div></div>";
  html += "</div></div>";

  // Control
  html += "<div class='card'><h2>Control</h2><div class='buttonGrid'>";
  html += "<a href='/on'><button class='on'>TURN ON</button></a>";
  html += "<a href='/off'><button class='off'>TURN OFF</button></a>";
  html += "<a href='/reset'><button class='reset'>RESET FAULT</button></a>";
  html += "</div></div>";

  html += "<p class='small'>ESP32 Smart Electrical Safety Prototype</p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void handleON()    { if (!fault) relayON();   server.sendHeader("Location","/"); server.send(303); }
void handleOFF()   { relayOFF();              server.sendHeader("Location","/"); server.send(303); }
void handleRESET() { if (fault) resetFault(); server.sendHeader("Location","/"); server.send(303); }


// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=================================");
  Serial.println(" SMART ELECTRICAL SAFETY SYSTEM ");
  Serial.println("=================================");

  pinMode(RELAY_PIN,      OUTPUT);
  pinMode(GREEN_LED_PIN,  OUTPUT);
  pinMode(RED_LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN,     OUTPUT);
  pinMode(RESET_BUTTON,   INPUT_PULLUP);
  pinMode(ISD_TRIGGER,    OUTPUT);

  digitalWrite(RELAY_PIN,     HIGH);   // Relay OFF on start
  digitalWrite(GREEN_LED_PIN, HIGH);   // Green ON = system ready
  digitalWrite(RED_LED_PIN,   LOW);
  digitalWrite(BUZZER_PIN,    LOW);
  digitalWrite(ISD_TRIGGER,   LOW);

  analogReadResolution(12);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!ina219.begin()) Serial.println("INA219 NOT FOUND!");
  else                 Serial.println("INA219 found at 0x40");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  unsigned long t = millis();
  while (WiFi.status() != WL_CONNECTED && millis()-t < 20000) {
    delay(500); Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi CONNECTED!");
    Serial.print("Open: http://"); Serial.println(WiFi.localIP());
    server.on("/",      handleRoot);
    server.on("/on",    handleON);
    server.on("/off",   handleOFF);
    server.on("/reset", handleRESET);
    server.begin();
  } else {
    Serial.println("WiFi FAILED — running offline");
  }

  Serial.println("System READY");
}


// =====================================================
// LOOP
// =====================================================
void loop()
{
  if (WiFi.status() == WL_CONNECTED) server.handleClient();

  temperature1 = readNTC(NTC1_PIN);
  temperature2 = readNTC(NTC2_PIN);
  deltaT       = temperature1 - temperature2;
  readINA219();

  // Auto protection
  if (!fault && temperature1 >= TRIP_TEMP) activateFault();

  // Physical reset button
  if (digitalRead(RESET_BUTTON) == LOW) {
    delay(50);
    if (digitalRead(RESET_BUTTON) == LOW) {
      if (fault) resetFault();
      while (digitalRead(RESET_BUTTON) == LOW) delay(10);
    }
  }

  // LED status
  digitalWrite(GREEN_LED_PIN, fault ? LOW  : HIGH);
  digitalWrite(RED_LED_PIN,   fault ? HIGH : LOW);

  updateBuzzer();

  // Serial status every 2s
  if (millis() - lastStatus >= 2000) {
    lastStatus = millis();
    Serial.println("\n------- STATUS -------");
    Serial.printf("Voltage:  %.2f V\n",  voltage);
    Serial.printf("Current:  %.1f mA\n", current_mA);
    Serial.printf("Power:    %.2f W\n",  power_mW / 1000.0);
    Serial.printf("NTC-1:    %.2f C\n",  temperature1);
    Serial.printf("NTC-2:    %.2f C\n",  temperature2);
    Serial.printf("Delta-T:  %.2f C\n",  deltaT);
    Serial.printf("Status:   %s\n",      fault ? "FAULT" : "NORMAL");
    Serial.printf("Runtime:  %lu s\n",   getRuntime());
    Serial.println("---------------------");
  }

  delay(10);
}

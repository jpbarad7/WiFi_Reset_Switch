#define BLYNK_TEMPLATE_ID "TMPL2EiXcmEpB"
#define BLYNK_TEMPLATE_NAME "WiFi Reset Switch"
#define BLYNK_AUTH_TOKEN "KsWYguR6YhCCw5sAdMlquzKZzY9bBIvv"

#define BLYNK_RED  "#D3435C"
#define BLYNK_GREEN "#23C48E"
#define BLYNK_GRAY "#D3D3D3"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int Reset_armed;
int Reset_NOW;

bool resetInProgress = false;
unsigned long resetStartTime = 0;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "BR Guest";
char pass[] = "Pamma355!";

void Reset();
void Reset_ARMED();

BLYNK_CONNECTED() {
  Blynk.virtualWrite(V0, LOW);
  Blynk.virtualWrite(V1, LOW);
  Blynk.virtualWrite(V2, LOW);
}

BLYNK_WRITE(V0) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    Blynk.setProperty(V0, "offBackColor", BLYNK_RED);
    Blynk.setProperty(V0, "offLabel", "Disabled");
    Reset_armed = 1;
  } 
  else {
    Blynk.setProperty(V0, "onBackColor", BLYNK_GREEN);
    Blynk.setProperty(V0, "onLabel", "Enabled");
    Reset_armed = 0;
  }
}

BLYNK_WRITE(V1) {
  int pinValue = param.asInt();
  if (pinValue == 1) {
    Reset_NOW = 1;
  }
  else {
    Reset_NOW = 0;
  }
}

BLYNK_WRITE(V2) {
  int pinValue = param.asInt();
}  

void setup() {
  WiFi.begin(ssid, pass);
  Blynk.config(auth);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(100);
  } 

  pinMode (4, OUTPUT);

  Reset_armed = 0;
  Reset_NOW = 0;
  resetInProgress = false;
  digitalWrite(4, LOW);         // make sure the pin is off
  Blynk.virtualWrite(V0, 0);    // ensure V0 is LOW
  Blynk.virtualWrite(V1, 0);    // ensure V1 is LOW
  Blynk.virtualWrite(V2, 0);    // clear output
}

void loop() {
  Blynk.run();

  // --- V1 BUTTON UI LOGIC ---
  if (Reset_armed == 0) {
    // Case 1: Disarmed – gray, disabled, standby
    Blynk.setProperty(V1, "isDisabled", true);
    Blynk.setProperty(V1, "offLabel", "Standby");
    Blynk.setProperty(V1, "offBackColor", BLYNK_GRAY);
  }
  else if (Reset_armed == 1 && Reset_NOW == 0) {
    // Case 2: Armed, waiting for press – red, enabled
    Blynk.setProperty(V1, "isDisabled", false);
    Blynk.setProperty(V1, "offLabel", "Press to Reset");
    Blynk.setProperty(V1, "offBackColor", BLYNK_RED);
    Blynk.setProperty(V1, "onBackColor", BLYNK_GREEN);
  }
  else if (Reset_armed == 1 && Reset_NOW == 1) {
    // Case 3: Button pressed – green, enabled, trigger reset
    Blynk.setProperty(V1, "isDisabled", false);
    Blynk.setProperty(V1, "offLabel", "Reset in Progress");
    Blynk.setProperty(V1, "offBackColor", BLYNK_GREEN);
    Blynk.setProperty(V1, "onBackColor", BLYNK_GREEN);

    // Initiate reset
    Reset_armed = 0;
    Reset_NOW = 0;
    Reset();
  }

  // --- END RESET AFTER 15s ---
  if (resetInProgress && millis() - resetStartTime >= 15000) {
    digitalWrite(4, LOW);
    Blynk.virtualWrite(V0, 0); // Disarm
    Blynk.virtualWrite(V1, 0); // Reset button to LOW
    resetInProgress = false;
  }
}



void Reset() {
  Blynk.virtualWrite(V2, 1);
  digitalWrite(4, HIGH);
  resetInProgress = true;
  resetStartTime = millis();
}

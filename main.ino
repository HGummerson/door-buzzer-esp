#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// wifi
const char* SSID = wifi_ssid;
const char* PASS = wifi_pass;

// relay url and polling
const char* RELAY_URL = relay_url;
const unsigned long POLL_MS = 3000;
unsigned long lastPoll = 0;

// servo setup
const int SERVO_PIN = 18;        // SG90 signal -> GPIO 18
const int SERVO_FREQ = 50;       
const int PULSE_MIN = 500;       
const int PULSE_MAX = 2400;      

int ANGLE_IDLE    = 20;   // not pressing
int ANGLE_PRESS   = 95;   // pressing
int PRESS_HOLD_MS = 2000;  // hold time on buzzer
int COOLDOWN_MS   = 2000; // lockout between presses

// globals
Servo servo;
unsigned long lastPress = 0;

// moving arm 
void smoothMove(int fromA, int toA, int stepDelayMs = 5) {
  fromA = constrain(fromA, 0, 180);
  toA   = constrain(toA,   0, 180);
  int step = (toA > fromA) ? 1 : -1;
  for (int a = fromA; a != toA; a += step) {
    servo.write(a);
    delay(stepDelayMs);
  }
  servo.write(toA);
}

// the actual press
void goPress() {
  unsigned long now = millis();
  if (now - lastPress < (unsigned long)COOLDOWN_MS) return;
  lastPress = now;

  int current = servo.read();
  if (current < 0 || current > 180) current = ANGLE_IDLE;

  // debugging
  //Serial.println("[servo] PRESS");
  smoothMove(current, ANGLE_PRESS);
  delay(PRESS_HOLD_MS);
  smoothMove(ANGLE_PRESS, ANGLE_IDLE);

  // debugging
  //Serial.println("[servo] DONE");
}

// poll Cloudflare relay
void pollRelay() {
  if (millis() - lastPoll < POLL_MS) return;
  lastPoll = millis();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[wifi] disconnected");
    return;
  }

  HTTPClient http;
  http.begin(RELAY_URL);
  int code = http.GET();
  if (code == 200) {
    String resp = http.getString();
    if (resp.indexOf("buzz") >= 0) {
      Serial.println("[relay] BUZZ received");
      goPress();
    } else {
      Serial.println("[relay] idle");
    }
  } else {
    Serial.print("[relay] HTTP error: ");
    Serial.println(code);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);

  // servo init
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo.setPeriodHertz(SERVO_FREQ);
  servo.attach(SERVO_PIN, PULSE_MIN, PULSE_MAX);
  servo.write(ANGLE_IDLE);
  delay(300);

  // wifi connect
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }
  Serial.print("[wifi] IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  pollRelay();           
}

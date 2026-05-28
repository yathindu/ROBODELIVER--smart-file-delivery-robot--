
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// WiFi
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourPassword"

// Firebase
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret"

// Pins
#define TRIG_PIN D3
#define ECHO_PIN D4
#define BUZZER_PIN D5
#define SERVO_PIN D6

#define OBSTACLE_DIST 20  // cm

// Objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo boxServo;
FirebaseData fbData;
FirebaseConfig config;
FirebaseAuth auth;

// Variables
String robotPath = "/robots/robot1";
bool obstacleDetected = false;
unsigned long lastClearTime = 0;

// ==================== SETUP ====================

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Servo
  boxServo.attach(SERVO_PIN);
  boxServo.write(0);
  
  // LCD
  Wire.begin(D2, D1);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Starting...");
  
  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  lcd.clear();
  lcd.print("WiFi OK!");
  delay(500);
  
  // Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  lcd.clear();
  lcd.print("Ready!");
  lcd.setCursor(0, 1);
  lcd.print("At: Base Station");
}

// ==================== ULTRASONIC SENSOR ====================

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long dur = pulseIn(ECHO_PIN, HIGH, 30000);
  long dist = dur * 0.034 / 2;
  
  if (dist == 0 || dist > 400) {
    return 999;
  }
  return dist;
}

// ==================== OBSTACLE DETECTION ====================

void checkObstacle() {
  long dist = getDistance();
  
  if (dist < OBSTACLE_DIST && dist > 0) {
    if (!obstacleDetected) {
      obstacleDetected = true;
      
      // Buzzer beep
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      
      // LCD
      lcd.clear();
      lcd.print("!! OBSTACLE !!");
      lcd.setCursor(0, 1);
      lcd.print("Dist: ");
      lcd.print(dist);
      lcd.print(" cm");
      
      // Pause robot
      Serial.println("PAUSE");
      Firebase.setBool(fbData, robotPath + "/obstacleDetected", true);
    }
    lastClearTime = 0;

  } else {
    if (obstacleDetected) {
      if (lastClearTime == 0) lastClearTime = millis();
      
      // Clear for 1 second = resume
      if (millis() - lastClearTime > 1000) {
        obstacleDetected = false;
        Serial.println("RESUME");
        Firebase.setBool(fbData, robotPath + "/obstacleDetected", false);
        showStatus();
      }
    }
  }
}

// ==================== FIREBASE ====================

void checkFirebase() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck < 500) return;
  lastCheck = millis();
  
  if (Firebase.getString(fbData, robotPath + "/command")) {
    String cmd = fbData.stringData();
    
    if (cmd == "START_MISSION") {
      if (Firebase.getString(fbData, robotPath + "/queue")) {
        String dest = fbData.stringData();
        Firebase.setString(fbData, robotPath + "/command", "NONE");
        boxServo.write(0);
        startMission(dest);
      }
    }
    else if (cmd == "RETURN_HOME") {
      Firebase.setString(fbData, robotPath + "/command", "NONE");
      boxServo.write(0);
      startMission("Base Station");
    }
  }
}

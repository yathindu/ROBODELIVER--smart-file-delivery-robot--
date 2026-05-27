// ===== NodeMCU - Smart File Delivery Robot =====
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
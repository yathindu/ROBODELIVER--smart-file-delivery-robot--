// IR Sensor Pins
#define S0 A0  // Rightmost
#define S1 A1
#define S2 A2  // Center
#define S3 A3
#define S4 A4  // Leftmost

#define THRESHOLD 650  // Below = black line

// States
enum State { IDLE, FOLLOWING, AT_JUNCTION, ARRIVED, WAITING, PAUSED };
State state = IDLE;
State stateBeforePause = IDLE;

// Sensor values (0 = black, 1 = white)
int b0, b1, b2, b3, b4;

// Navigation
int lastDirection = 0;  // 0=forward, 1=left, 2=right
String pendingTurn = "";
int lostCount = 0;

void setup() {
  // Motor pins
  pinMode(12, OUTPUT); pinMode(11, OUTPUT);
  pinMode(10, OUTPUT); pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);  pinMode(9, OUTPUT);

  Serial.begin(9600);
  delay(2000);
  stop();
  Serial.println("READY");
}

void loop() {
  checkSerial();
  readSensors();
  
  switch (state) {
    case IDLE:
    case ARRIVED:
    case WAITING:
    case PAUSED:
      stop();
      break;
    case FOLLOWING:
      followLine();
      break;
    case AT_JUNCTION:
      if (pendingTurn != "") executeTurn();
      break;
  }
}

// ==================== SERIAL COMMUNICATION ====================

void checkSerial() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    processCommand(cmd);
  }
}

void processCommand(String cmd) {
  if (cmd == "GO") {
    lostCount = 0;
    state = FOLLOWING;
  }
  else if (cmd == "STOP") {
    stop();
    state = WAITING;
  }
  else if (cmd == "PAUSE") {
    if (state == FOLLOWING || state == AT_JUNCTION) {
      stateBeforePause = state;
      state = PAUSED;
      stop();
    }
  }
  else if (cmd == "RESUME") {
    if (state == PAUSED) {
      state = stateBeforePause;
    }
  }
  else if (cmd.startsWith("TURN:")) {
    pendingTurn = cmd.substring(5);
  }
}

// ==================== SENSOR READING ====================

void readSensors() {
  b0 = (analogRead(S0) < THRESHOLD) ? 0 : 1;
  b1 = (analogRead(S1) < THRESHOLD) ? 0 : 1;
  b2 = (analogRead(S2) < THRESHOLD) ? 0 : 1;
  b3 = (analogRead(S3) < THRESHOLD) ? 0 : 1;
  b4 = (analogRead(S4) < THRESHOLD) ? 0 : 1;
}

// ==================== LINE FOLLOWING ALGORITHM ====================

void followLine() {
  
  // T-junction (all sensors see black)
  if (b0 == 0 && b1 == 0 && b2 == 0 && b3 == 0 && b4 == 0) {
    stop();
    delay(100);
    lostCount = 0;
    Serial.println("JUNCTION");
    state = AT_JUNCTION;
    return;
  }

  // Right L-turn
  if (b0 == 0 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 1) {
    stop();
    delay(100);
    lostCount = 0;
    Serial.println("JUNCTION");
    state = AT_JUNCTION;
    return;
  }

  // Left L-turn
  if (b2 == 0 && b3 == 0 && b4 == 0 && b1 == 1 && b0 == 1) {
    stop();
    delay(100);
    lostCount = 0;
    Serial.println("JUNCTION");
    state = AT_JUNCTION;
    return;
  }

  // Center only - forward
  if (b2 == 0 && b0 == 1 && b1 == 1 && b3 == 1 && b4 == 1) {
    forward();
    lastDirection = 0;
    lostCount = 0;
  }
  // Far right - sharp right
  else if (b0 == 0 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 1) {
    right2();
    lastDirection = 2;
    lostCount = 0;
  }
  // Right - gentle right
  else if (b0 == 1 && b1 == 0 && b2 == 1 && b3 == 1 && b4 == 1) {
    right1();
    lastDirection = 2;
    lostCount = 0;
  }
  // Right-center - gentle left
  else if (b0 == 1 && b1 == 0 && b2 == 0 && b3 == 1 && b4 == 1) {
    left1();
    lastDirection = 2;
    lostCount = 0;
  }
  // Left - gentle left
  else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 0 && b4 == 1) {
    left1();
    lastDirection = 1;
    lostCount = 0;
  }
  // Left-center - gentle right
  else if (b0 == 1 && b1 == 1 && b2 == 0 && b3 == 0 && b4 == 1) {
    right1();
    lastDirection = 1;
    lostCount = 0;
  }
  // Far left - sharp left
  else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 0) {
    left2();
    lastDirection = 1;
    lostCount = 0;
  }
  // All white - line lost
  else if (b0 == 1 && b1 == 1 && b2 == 1 && b3 == 1 && b4 == 1) {
    lostCount++;
    
    // Lost too long = arrived
    if (lostCount > 100) {
      stop();
      Serial.println("ARRIVED");
      state = ARRIVED;
      return;
    }
    
    // Try to find line
    if (lostCount < 30) {
      forward();
    }
    else if (lastDirection == 1) {
      left2();
    }
    else if (lastDirection == 2) {
      right2();
    }
    else {
      stop();
    }
  }
  else {
    forward();
    lostCount = 0;
  }
}

// ==================== TURN EXECUTION ====================

void executeTurn() {
  
  if (pendingTurn == "LEFT") {
    forward();
    delay(200);
    unsigned long t = millis();
    while (millis() - t < 2000) {
      spinLeft();
      readSensors();
      if (millis() - t > 300 && b2 == 0) break;
    }
  }
  else if (pendingTurn == "RIGHT") {
    forward();
    delay(200);
    unsigned long t = millis();
    while (millis() - t < 2000) {
      spinRight();
      readSensors();
      if (millis() - t > 300 && b2 == 0) break;
    }
  }
  else if (pendingTurn == "STRAIGHT") {
    forward();
    delay(400);
  }
  
  stop();
  delay(100);
  pendingTurn = "";
  state = FOLLOWING;
}

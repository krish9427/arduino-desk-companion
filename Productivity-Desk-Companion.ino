#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// --- HARDWARE PIN ASSIGNMENTS ---
const int BTN_1 = 7;     // Action Button (Add Hour / Log Water)
const int BTN_2 = 8;     // Toggle Button (Start/Pause Timer)
const int BTN_3 = 9;     // Reset Button
const int BUZZER = 10;   // 2-Wire Buzzer

// --- INITIALIZE SENSORS & DISPLAY ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// --- SYSTEM STATES ---
int currentMode = 0; // 0 = Timer, 1 = Water Tracker, 2 = Temperature Monitor
unsigned long lastModeChangeTime = 0;
const int SHAKE_THRESHOLD = 14; // How hard you need to shake the box to change modes

// --- MODE VARIABLES ---
unsigned long timerDuration = 0; 
unsigned long timerStartTime = 0;
unsigned long timerRemaining = 0;
bool timerRunning = false;

int waterCount = 0;
const int WATER_GOAL = 8; 
unsigned long waterResetTimer = 0;

unsigned long lastTempCheck = 0;
float currentTempCelsius = 24.5;

float lastX = 0, lastY = 0;

// --- BUTTON DEBOUNCE FILTER ---
// Ensures clicks are registered cleanly without freezing
bool isButtonPressed(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(50); // Wait for physical button contacts to settle
    if (digitalRead(pin) == LOW) {
      while(digitalRead(pin) == LOW); // Wait for button release
      return true;
    }
  }
  return false;
}

void setup() {
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Companion Box");
  lcd.setCursor(0, 1);
  lcd.print("3 Modes Active");

  if(!accel.begin()) {
    lcd.clear();
    lcd.print("No ADXL345 Found");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

  sensors_event_t event;
  accel.getEvent(&event);
  lastX = event.acceleration.x;
  lastY = event.acceleration.y;

  // Wake up Analog-to-Digital features for internal temp sensing
  ADCSRA |= _BV(ADEN); 
  delay(10);

  tone(BUZZER, 2000, 100);
  delay(800);
  lcd.clear();
}

void loop() {
  unsigned long currentTime = millis();
  sensors_event_t event;
  accel.getEvent(&event);

  // --- 1. SHAKE DETECTION TO SWITCH MODES ---
  float deltaX = abs(event.acceleration.x - lastX);
  float deltaY = abs(event.acceleration.y - lastY);
  float sideToSideShake = deltaX + deltaY;
  
  lastX = event.acceleration.x;
  lastY = event.acceleration.y;

  // Cycles through 3 pages smoothly: 0, 1, 2
  if (sideToSideShake > SHAKE_THRESHOLD && (currentTime - lastModeChangeTime > 1200)) {
    currentMode = (currentMode + 1) % 3; 
    lastModeChangeTime = currentTime;
    tone(BUZZER, 1300, 50); 
    lcd.clear();
  }

  // --- 2. BACKGROUND 24-HOUR CLOCK ---
  if (currentTime - waterResetTimer >= 86400000) {
    waterCount = 0;
    waterResetTimer = currentTime;
  }

  // --- 3. RUN ACTIVE FEATURE MODE ---
  switch (currentMode) {
    case 0: handleTimerMode(currentTime); break;
    case 1: handleWaterMode(); break;
    case 2: handleTempMode(currentTime); break;
  }
  
  delay(20); 
}

// --- ACTIVE SUBMODULE PAGES ---

void handleTimerMode(unsigned long currentTime) {
  if (timerRunning) {
    unsigned long elapsed = currentTime - timerStartTime;
    if (elapsed >= timerDuration) {
      timerDuration = 0;
      timerRunning = false;
      lcd.clear();
      lcd.print("   TIME UP!!   ");
      for(int i=0; i<4; i++) {
        tone(BUZZER, 1200, 300); delay(400);
      }
      lcd.clear();
    } else {
      timerRemaining = timerDuration - elapsed;
    }
  }

  // Button 1: Add 1 Hour
  if (isButtonPressed(BTN_1)) { 
    if (!timerRunning) {
      timerDuration += 3600000;
      timerRemaining = timerDuration;
      tone(BUZZER, 1300, 40);
      lcd.clear();
    }
  }
  
  // Button 2: Toggle Start/Pause
  if (isButtonPressed(BTN_2)) { 
    if (timerDuration > 0) {
      timerRunning = !timerRunning;
      if (timerRunning) {
        timerStartTime = currentTime - (timerDuration - timerRemaining);
      }
      tone(BUZZER, 1100, 60);
      lcd.clear();
    }
  }
  
  // Button 3: Reset Timer
  if (isButtonPressed(BTN_3)) { 
    timerDuration = 0;
    timerRemaining = 0;
    timerRunning = false;
    tone(BUZZER, 700, 100);
    lcd.clear();
  }

  unsigned long seconds = timerRemaining / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  seconds %= 60;
  minutes %= 60;

  lcd.setCursor(0, 0);
  lcd.print("MODE: FocusTimer");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if(hours < 10) lcd.print("0"); lcd.print(hours); lcd.print(":");
  if(minutes < 10) lcd.print("0"); lcd.print(minutes); lcd.print(":");
  if(seconds < 10) lcd.print("0"); lcd.print(seconds);
  if(!timerRunning && timerDuration > 0) lcd.print(" (P)");
  else lcd.print("    ");
}

void handleWaterMode() {
  // Button 1: Add Glass
  if (isButtonPressed(BTN_1)) {
    waterCount++;
    tone(BUZZER, 1700, 60);
    lcd.clear();
  }
  
  // Button 3: Deduct Glass
  if (isButtonPressed(BTN_3)) {
    if(waterCount > 0) waterCount--;
    tone(BUZZER, 800, 60);
    lcd.clear();
  }

  lcd.setCursor(0, 0);
  lcd.print("MODE: Water Log ");
  lcd.setCursor(0, 1);
  lcd.print("Glasses: ");
  lcd.print(waterCount);
  lcd.print("/");
  lcd.print(WATER_GOAL);
  if (waterCount >= WATER_GOAL) lcd.print(" Done!");
  else lcd.print("      ");
}

void handleTempMode(unsigned long currentTime) {
  if (currentTime - lastTempCheck >= 2000 || lastTempCheck == 0) {
    // Select internal temperature read channel
    ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3)); 
    delay(5);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));
    unsigned int rawADC = ADCW;

    if (rawADC > 0) {
      currentTempCelsius = (rawADC - 324.31) / 1.22;
      if(currentTempCelsius < 10 || currentTempCelsius > 45) {
         currentTempCelsius = 24.5; 
      }
    }
    // Instantly reset registers back to normal so they don't block digital reads
    ADMUX = _BV(REFS0); 
    lastTempCheck = currentTime;
  }

  lcd.setCursor(0, 0);
  lcd.print("MODE: Desk Temp ");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(currentTempCelsius, 1);
  lcd.print((char)223); 
  lcd.print("C    ");
}
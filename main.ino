#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RED 7
#define GREEN 6
#define BLUE 5
#define IN 8
#define OUT 9

#define RESET 13

int in = 0;
int out = 0;
int inside = 0;

long IN_TIME = -1;
long OUT_TIME = -1;

bool InTrigged = false;
bool outTrigged = false;

void setup() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  pinMode(IN, INPUT);
  pinMode(OUT, INPUT);

  pinMode(RESET, INPUT);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  displayLCD();
}

void SET_LED(int R, int G, int B) {
    digitalWrite(RED, R);
    digitalWrite(GREEN, G);
    digitalWrite(BLUE, B);
}

void displayLCD() {
  lcd.setCursor(0, 0);
  lcd.print("IN = " + String(in));

  lcd.setCursor(8, 0);
  lcd.print("OUT = " + String(out));

  int inside = in - out > 0 ? in - out : 0;

  lcd.setCursor(0, 1);
  lcd.print("INSIDE = " + String(inside));

  showLED(inside);
}

void showLED(int inside) {
  if (inside < 5) {
    SET_LED(0, 255, 0);
  } else if (inside < 10) {
    SET_LED(245, 138, 0);
  } else {
    SET_LED(255, 0, 0);
  }
}

bool isInTrigged() {
  return digitalRead(IN) == LOW;
}

bool isOutTrigged() {
  return digitalRead(OUT) == HIGH;
}

bool isTimeGreaterThan(long t) {
  return millis() - t >= 0;
}

bool isStateVoid() {
  return !InTrigged && !outTrigged;
}

void resetState() {
  IN_TIME = 0;
  OUT_TIME = 0;

  InTrigged = false;
  outTrigged = false;
}

void CheckIn() {
  bool isIn = isInTrigged();
  bool isOut = isOutTrigged();

  if (isIn && isStateVoid()) {
    IN_TIME = millis();

    InTrigged = true;
  }

  if (isOut && isTimeGreaterThan(IN_TIME) && InTrigged && !outTrigged) {
    in++;
    
    resetState();
    delay(200);
  }
}

void CheckOut() {
  bool isIn = isInTrigged();
  bool isOut = isOutTrigged();

  if (isOut && isStateVoid()) {
    OUT_TIME = millis();

    outTrigged = true;
  } 

  if (isIn && isTimeGreaterThan(OUT_TIME) && outTrigged && !InTrigged) {
    if (in - out > 0) {
      out++;
    }

    resetState();
    delay(200);
  }
}

void loop() {
  if (isInTrigged() && isOutTrigged()) {
      return;
  }

  CheckIn();
  CheckOut();

  displayLCD();
}

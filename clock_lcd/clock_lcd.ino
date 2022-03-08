#include "LiquidCrystal.h"
#include "RTClib.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RTC_DS1307 rtc;

TimeSpan interval(0, 1, 0, 0); // Once every hour
DateTime last;

#define LCD_BACKLIGHT 10
#define LCD_BUTTONS 0

void setup() {
  pinMode(LCD_BACKLIGHT, OUTPUT);
  pinMode(LCD_BUTTONS, INPUT);
  // put your setup code here, to run once:
  lcdOn();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  
  if (! rtc.begin()) {
    lcd.print("Could not find RTC...");
    while (1) delay(10);
  }

  last = rtc.now();

  lcd.print("Ready!");
  delay(1000);
}

void loop() {
  int read;
  
  DateTime now = rtc.now();
  DateTime off (now + TimeSpan(0,0,0,10));
  DateTime last_print = now;

  lcd.clear();
  updatePrint(now);
  
  while ((now = rtc.now()) < off) {
    if (now.second() != last_print.second()) {
      last_print = now;
      updatePrint(now);
    }
    read = analogRead(LCD_BUTTONS);
    if (read <= 920) {
      off = DateTime(now + TimeSpan(0,0,0,10));
      lcd.clear();
      updatePrint(now);
      
      if (read <= 600) {
        logIt(true);
        lcd.setCursor(0, 1);
        lcd.print("Logged current");
        delay(1000);
        lcd.clear();
      }
    }
    delay(10);
  }

  // Screen off mode

  lcdOff();
  read = analogRead(LCD_BUTTONS);
  while (read > 920) {
    if (needsLog()) {
      last = rtc.now();
      logIt(false);
    }
    read = analogRead(LCD_BUTTONS);
    delay(10);
  }
  lcdOn();
}

void lcdOn() {
  lcd.display();
  digitalWrite(LCD_BACKLIGHT, HIGH);
}

void lcdOff() {
  //lcd.clear();
  lcd.noDisplay();
  digitalWrite(LCD_BACKLIGHT, LOW);
}

bool needsLog() {
  return rtc.now() >= last + interval;
}

void logIt(bool user) {
  
}

void updatePrint(DateTime now) {
  lcd.setCursor(0,0);
  lcd.print("00:00:00");
  lcd.setCursor((now.hour() < 10) ? 1 : 0, 0);
  lcd.print(now.hour());
  lcd.setCursor((now.minute() < 10) ? 4 : 3, 0);
  lcd.print(now.minute());
  lcd.setCursor((now.second() < 10) ? 7 : 6, 0);
  lcd.print(now.second());
}

#include "LiquidCrystal.h"
#include "RTClib.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RTC_DS1307 rtc;

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

  lcd.print("Ready!");
  delay(1000);
}

void loop() {
  DateTime now = rtc.now();
  DateTime off (now + TimeSpan(0,0,0,10));
  
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print("00:00:00");
  lcd.setCursor((now.hour() < 10) ? 1 : 0, 0);
  lcd.print(now.hour());
  lcd.setCursor((now.minute() < 10) ? 4 : 3, 0);
  lcd.print(now.minute());
  lcd.setCursor((now.second() < 10) ? 7 : 6, 0);
  lcd.print(now.second());

  lcd.setCursor(0,1);

  while ((now = rtc.now()) < off) {
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
  
    lcd.print("00:00:00");
    lcd.setCursor((now.hour() < 10) ? 1 : 0, 0);
    lcd.print(now.hour());
    lcd.setCursor((now.minute() < 10) ? 4 : 3, 0);
    lcd.print(now.minute());
    lcd.setCursor((now.second() < 10) ? 7 : 6, 0);
    lcd.print(now.second());
  }

  lcdOff();
  int read = analogRead(LCD_BUTTONS);
  while (read > 920 or read < 600) {
    read = analogRead(LCD_BUTTONS);
    delay(10);
  }
  lcdOn();
}

void lcdOn() {
  void display();
  digitalWrite(LCD_BACKLIGHT, HIGH);
}

void lcdOff() {
  void noDisplay();
  digitalWrite(LCD_BACKLIGHT, LOW);
}

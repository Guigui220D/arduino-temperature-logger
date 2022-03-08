#include "LiquidCrystal.h"
#include "RTClib.h"
#include "SD.h"
#include "Wire.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RTC_DS1307 rtc;

TimeSpan interval(0, 1, 0, 0); // Once every hour
DateTime last;

File logFile;

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
    lcd.print("Cant find RTC");
    while(1) delay(10);
  }

  char fileName[] = "LOG00.CSV";
  for (uint8_t i = 0; i < 99; i++) {
    fileName[3] = i/10 + '0';
    fileName[4] = i%10 + '0';

    if (!SD.exists(fileName)) {
      logFile = SD.open(fileName, FILE_WRITE);
      break;
    }
  }

  Wire.begin();
  if (!rtc.begin()) {
    lcd.print("Cant open file");
    while(1) delay(10);
  }

  logFile.println("time,from");
  logFile.flush();
  
  logIt(false);
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
  DateTime now = rtc.now();

  logFile.print(now.day(), DEC);
  logFile.print("/");
  logFile.print(now.month(), DEC);
  logFile.print("/");
  logFile.print(now.year(), DEC);
  logFile.print(" ");
  logFile.print(now.hour(), DEC);
  logFile.print(":");
  logFile.print(now.minute(), DEC);
  logFile.print(":");
  logFile.print(now.second(), DEC);

  logFile.print(",");
  if (user) {
    logFile.print("user");
  } else {
    logFile.print("hourly");
  }
  logFile.flush();
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

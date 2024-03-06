// DS18B20 Temperature Sensor to Arduino with LCD , button , debounce
////////////////////////// LCD ////////////////////////////
#include <LiquidCrystal.h>
LiquidCrystal lcd(4, 5, 6, 7, 8, 9); // (rs, en, d4, d5, d6, d7)
///////////////////////////////////////////////////////////
////////////////////// Button /////////////////////////////
int ledState = HIGH;
int presentButtonState;
int Button = 10;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 200;
//////////////////////////////////////////////////////////
///////////////////////// DS18B20() //////////////////////
#include <Wire.h>
#include <OneWire.h>
OneWire  ds(2);
byte i;
byte type_s = 0;
byte data[12];
byte addr[8];
double tempC = 0;
double tempF = 0;
//////////////////////////////////////////////////////////
////////////////////////////// SETUP /////////////////////
void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16,2);
  lcd.clear();
  pinMode(Button, INPUT_PULLUP);
}
//////////////////////////////////////////////////////////
//////////////////////////// DS18B20() ///////////////////
void DS18B20(){
  ds.search(addr);
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 0);        
  ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  tempC = (float)raw / 16.0;
  tempF = (tempC * 9.0) / 5.0 + 32.0;
}
/////////////////////////////////////////////////////////////////////
///////////////////////////////// LOOP //////////////////////////////
void loop() {
  DS18B20();
  presentButtonState = digitalRead(Button);
  if ( (millis() - lastDebounceTime) > debounceDelay) {
    if(presentButtonState == LOW) {
      ledState = !ledState;
      digitalRead(ledState);
      lastDebounceTime = millis();
    }
  }
  if (ledState == HIGH){
    lcd.setCursor(0,0);
    lcd.print("Temperature in C");
    lcd.setCursor(0,1);
    lcd.print(tempC);
    lcd.print(" degree");
  } else {
    lcd.setCursor(0,0);
    lcd.print("Temperature in F");
    lcd.setCursor(0,1);
    lcd.print(tempF);
    lcd.print(" degree");
  } 
}
/////////////////////////////////////////////////////////////////////
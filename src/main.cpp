#include <Arduino.h>
#include "ADNS5050.h"
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>
#include <BleMouse.h>
#include <Adafruit_I2CDevice.h>
//#include <BleCombo.h>

// The FT6206 uses hardware I2C (SCL/SDA)
int I2C_SDA = 9;
int I2C_SCL = 10;
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
// MOSI 11, SCK 12, MISO 13
#define TFT_CS 5
#define TFT_DC 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// DEFINE PINS:
// Mouse Sensor Pins (Can be any GPIO Pins)
int ms_sclk = 35;
int ms_sdio = 36;
int ms_select = 37;
int ms_reset = 38;

// Define Objects
ADNS5050 mouse_sensor(ms_sclk,ms_sdio,ms_select,ms_reset);
int dx;
int dy;
BleMouse bleMouse;

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup() {
  Serial.begin(115200);

  // Change I2C pins for touchscreen
  Wire.begin(I2C_SDA, I2C_SCL);

  // Start BLE control
  bleMouse.begin();
  //Keyboard.begin();
  //Mouse.begin();

  // Sync mouse sensor
  //Serial.println("Sensor Initialized");
  mouse_sensor.sync();
  Serial.println("Sensor Sync");
  while (!Serial) delay(10);

  
  tft.begin();

  if (! ctp.begin(40, &Wire)) {  // pass in 'sensitivity' coefficient and I2C bus
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1) delay(10);
  }

  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(ILI9341_BLACK);

  // Display buttons and shortcuts
  tft.fillRect(0,0,80,100, ILI9341_WHITE); // Left Button
  tft.fillRect(160,0,80,100, ILI9341_WHITE); // Right Button

  // Scroll Up
  tft.fillRect(81,0,78,49, ILI9341_WHITE); 
  tft.fillTriangle(120,10,90,40,150,40,ILI9341_BLACK);

  // Scroll Down
  tft.fillRect(81,50,78,50, ILI9341_WHITE); 
  tft.fillTriangle(120,90,90,60,150,60,ILI9341_BLACK);

  // Shortcuts
  // Copy Button
  tft.fillRect(0,240,79,59, ILI9341_WHITE); // Button
  tft.setCursor(18, 260);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("Copy");
  // Cut Button
  tft.fillRect(80,240,79,59, ILI9341_WHITE); // Button
  tft.setCursor(100, 260);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("Cut");
  // Paste Button
  tft.fillRect(160,240,79,59, ILI9341_WHITE); // Button
  tft.setCursor(170, 260);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
  tft.println("Paste");

}

void loop() {
  // Get and send movement data over bluetooth
  
  if(bleMouse.isConnected()) {
    // Get sensor data and send over BLE
    bleMouse.move(-mouse_sensor.dx(),-mouse_sensor.dy(),0);
  }
  else {
    delay(2000);
  }
  

 /*
  if(Keyboard.isConnected()) {
    Mouse.move(-mouse_sensor.dx(),-mouse_sensor.dy(),0);
  }
  else {
    delay(2000);
  }
  
  if (! ctp.touched()) {
    return;
  }
  */

  // Retrieve a point from touchscreen
  TS_Point p = ctp.getPoint();

  // flip it around to match the screen.
  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);

  // Compare touch location to button location
  // Left Button
  if (p.x < 80 && p.y < 100) {
    bleMouse.click(MOUSE_LEFT);
    //Mouse.click(MOUSE_LEFT);
    delay(50);
  }
  // Right Button
  if (p.x > 160 && p.y < 100) {
    bleMouse.click(MOUSE_RIGHT);
    //Mouse.click(MOUSE_RIGHT);
    delay(50);
  }
  // Scroll Up
  if (p.x > 80 && p.x < 160 && p.y < 50) {
    bleMouse.move(0,0,1);
    //Mouse.move(0,0,1);
    delay(50);
  }
  // Scroll Down
  if (p.x > 80 && p.x < 160 && p.y < 100 && p.y > 50) {
    bleMouse.move(0,0,-1);
    //Mouse.move(0,0,-1);
    delay(50);
  }

  //Shortcuts
  /*
  // Copy
  if (p.x > 80 && p.x < 160 && p.y < 100 && p.y > 50) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(C);
    delay(100);
    Keyboard.releaseAll();
  }
  // Cut
  if (p.x > 80 && p.x < 160 && p.y < 100 && p.y > 50) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(X);
    delay(100);
    Keyboard.releaseAll();
  }
  // Paste
  if (p.x > 80 && p.x < 160 && p.y < 100 && p.y > 50) {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(V);
    delay(100);
    Keyboard.releaseAll();
  }
  */


}


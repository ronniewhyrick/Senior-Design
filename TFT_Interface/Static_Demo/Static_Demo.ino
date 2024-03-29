// This code is designed for display on 2.0" TFT 240x320 pixel screen.
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <gfxfont.h>
#include <SoftwareSerial.h>

// Font definitions:
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// Color definitions:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Pin definitions: 
// See SPI Connections for more details (https://adafru.it/d5h)
//
// 3.3V - Voltage output
// GND - GND
// 13 - SCK (SPI clock)
// 11 - MOSI (SPI MOSI)
// 8* - CS (SPI Chip select)
// 7* - RST (Display Reset)
// 4* - D/C (SPI data/command select)
// *Can be changed to any pin

#define TFT_CS         8
#define TFT_RST        7
#define TFT_DC         4

// Interface the display using ANY TWO or THREE PINS
#define TFT_MOSI 11  // Data out
#define TFT_SCLK 13  // Clock out

// For the ST7789-based displays, we will use this call
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
SoftwareSerial Backpack(6,5);

const float xLat = -096.584159;
const float xLon =  39.190953;
float bLat = -096.58418;
float bLon =  39.19096;
float tLat = -096.58282;
float tLon = 39.18823;
String bLatS, bLonS;
String tLatS, tLonS;
String userLoc, trakLoc, lastLoc;
int x, x2, y, xLast, yLast, xLast2;
int i = 0;

void setup(void) {
  tft.init(240, 320);           
  tft.fillScreen(ST77XX_BLACK);
  Backpack.begin(9600);
  Serial.begin(9600);
  //tft.setRotation(2);
  startupScreen();
  initializeMap();
}

void loop() {
  while(i < 7){
      bLat -= 0.00002;
      bLon += 0.00001;
      tLat += 0.00002;
      tLon -= 0.00001;
      bLatS = String(bLat,5);
      bLonS = String(bLon,5);
      tLatS = String(tLat,5);
      tLonS = String(tLon,5);
      userLoc = bLonS + "N, " + bLatS + "W";
      trakLoc = tLonS + "N, " + tLatS + "W";
      Serial.print("userLoc: ");
      Serial.println(userLoc);
      Serial.print("trakLoc: ");
      Serial.println(trakLoc);
      updateCoordinates(userLoc); //Update user's coordinate position
      x -= 4;
      x2 = x - 85;
      y -= 12;
      xLast = x;
      yLast = y;
      xLast2 = xLast;
      updateTrakkers(x, y);
      //delay(4500);
  }
}

// Printout for the startup display
void startupScreen(){
  tft.setCursor(20, 110);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setFont(&FreeMonoBold24pt7b);
  tft.setTextWrap(true);
  tft.println("Welcome");
  tft.setCursor(95, 157);
  tft.println("to");
  tft.setCursor(20, 200);
  tft.println("PalPack");
  //delay(1000);
}

// Initialize map and prompts on display
void initializeMap(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0,0);
  uint16_t radius = 0;

  // Print system and coordinate prompts
  tft.setCursor(0,11);
  tft.println("System Status:");
  tft.setCursor(0,25);
  tft.println("Current coordinates:");

  // Print current system status and coordinates
  tft.setTextColor(ST77XX_WHITE);
  //tft.setFont(&FreeMonoBold9pt7b);
  tft.setCursor(155,11);
  tft.println("ON"); //replace with status variable
  
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
      tft.drawCircle(120, 164, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
  // Prints ring distance
  tft.setFont();
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(106, 258);
  tft.println("10.0 m");

  // Print user origin position on map
  tft.setTextColor(ST77XX_WHITE);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(114, 176);
  tft.setCursor(114, 170);
  tft.println("X");

  // Print available trakker prompt
  tft.setTextColor(ST77XX_GREEN);
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(0, 300);
  tft.println("Trakkers available:");
  tft.fillCircle(214,297,3,ST77XX_RED);
  tft.fillCircle(226,297,3,ST77XX_BLUE);
  //tft.fillCircle(235,297,3,ST77XX_YELLOW);
  tft.setCursor(0, 315);
  tft.println("Out of bounds:");
}

String updateCoordinates(String userLoc){
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_BLACK);
  tft.println(lastLoc);
  tft.setCursor(0,41);
  tft.setTextColor(ST77XX_WHITE);
  tft.println(userLoc);
  //Serial.print(F("curLoc: "));
  //Serial.println(userLoc);
  lastLoc = userLoc;
  //Serial.print("lastLoc: ");
  //Serial.println(lastLoc);
  return lastLoc;
}

void updateTrakkers(int x, int y){
  if(i < 12){
    tft.fillCircle(xLast,yLast,8,ST77XX_BLACK);
    tft.fillCircle(xLast2,yLast,8,ST77XX_BLACK);
    printMap();
    tft.fillCircle(x,y,8,ST77XX_RED);
    tft.fillCircle(x2,y,8,ST77XX_BLUE);
    xLast = x;
    xLast2 = x2;
    yLast = y;
  }
  else if(12 <= i < 14){
    tft.fillCircle(xLast,yLast,8,ST77XX_BLACK);
    tft.fillCircle(xLast2,yLast,8,ST77XX_BLACK);
    printMap();
    tft.fillCircle(x,y,8,ST77XX_RED);
    tft.fillCircle(158,312,3,ST77XX_BLUE);
    xLast = x;
    xLast2 = x2;
    yLast = y;
  }
}

void printMap(){
  uint16_t radius = 0;
  // Draws out sonar circles
  for (int16_t i = 0; i <= 5; i++) {
  //for (int16_t i = 0; i < 12; i++) {
    //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
      tft.drawCircle(120, 164, radius, ST77XX_GREEN);
      radius = radius + 22;
  }
}

